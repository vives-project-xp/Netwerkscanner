#include "time_sync.h"
#include "esp_timer.h"
#include "lwip/sockets.h"
#include <string.h>
#include <stdio.h>

#define TIME_SERVER_IP "10.20.10.24"
#define TIME_SERVER_PORT 80

static uint64_t baseServerTime = 0;
static uint64_t baseEspTime = 0;

// =======================
// PRIVATE
// =======================
static uint64_t GetTimeFromServer() {
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(TIME_SERVER_IP);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(TIME_SERVER_PORT);

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sock < 0) return 0;

    if (connect(sock, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) != 0) {
        close(sock);
        return 0;
    }

    const char *request =
        "GET /time HTTP/1.1\r\n"
        "Host: 10.20.10.24\r\n"
        "Connection: close\r\n\r\n";

    send(sock, request, strlen(request), 0);

    char buffer[512];
    int len = recv(sock, buffer, sizeof(buffer) - 1, 0);
    close(sock);

    if (len <= 0) return 0;

    buffer[len] = 0;

    char *jsonStart = strstr(buffer, "{");
    if (!jsonStart) return 0;

    uint64_t timestamp = 0;
    sscanf(jsonStart, "{\"timestamp\": %llu}", &timestamp);

    return timestamp;
}

// =======================
// PUBLIC
// =======================

void TimeSync::Init() {
    baseServerTime = 0;
    baseEspTime = 0;
}

void TimeSync::Sync() {
    uint64_t t = GetTimeFromServer();
    if (t == 0) return;

    baseServerTime = t;
    baseEspTime = esp_timer_get_time();

    printf("Time synced: %llu\n", t);
}

uint64_t TimeSync::Get() {
    if (baseServerTime == 0) return 0;

    uint64_t nowEsp = esp_timer_get_time();
    uint64_t delta = (nowEsp - baseEspTime) / 1000000ULL;

    return baseServerTime + delta;
}

bool TimeSync::IsValid() {
    return baseServerTime != 0;
}

// #include "time_sync.h"
// in app_main : TimeSync::Init();

// tijd gebruiken:
// uint64_t now = TimeSync::Get();
// printf("Unix time: %llu\n", now);

// Mogelijks:
//static int GetHour() {
//    return (Get() % 86400) / 3600;
// }