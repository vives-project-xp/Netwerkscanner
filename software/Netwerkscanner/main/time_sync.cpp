#include "time_sync.h"

#include <stdio.h>
#include <string.h>

#include "api.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "wifi_key.h"

static uint64_t baseServerTime = 0;
static uint64_t baseEspTime = 0;

// =======================
// PRIVATE
// =======================
static uint64_t GetTimeFromServer() {
    uint64_t timestamp = 0;
    char url_buffer[128];
    char response_buffer[256];
    
    snprintf(url_buffer, sizeof(url_buffer), "http://%s:%d/time", SERVER_IP, SERVER_PORT);

    esp_http_client_config_t config = {
        .url = url_buffer,
        .method = HTTP_METHOD_GET,
        .buffer_size = 1024,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_err_t err = esp_http_client_open(client, 0);

    if (err == ESP_OK) {
        esp_http_client_fetch_headers(client);
        int status_code = esp_http_client_get_status_code(client);
        int64_t content_len = esp_http_client_get_content_length(client);

        if (status_code == 200) {
            int len = esp_http_client_read_response(client, response_buffer, sizeof(response_buffer) - 1);
            
            printf("len: %d\n", len);
            if (len >= 0) {
                response_buffer[len] = '\0';
                printf("Response: %s\n", response_buffer);
                timestamp = GetJsonBigNumber(response_buffer, "timestamp");
            }
        } else {
            ESP_LOGE("/GET", "Server status error: %d", status_code);
        }
        
        ESP_LOGI("/GET", "HTTP GET Status = %d, content_length = %lld", status_code, content_len);
    } else {
        ESP_LOGE("/GET", "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    
    printf("timestamp: %llu\n", timestamp);
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

bool TimeSync::IsValid() { return baseServerTime != 0; }

// #include "time_sync.h"
// in app_main : TimeSync::Init();

// tijd gebruiken:
// uint64_t now = TimeSync::Get();
// printf("Unix time: %llu\n", now);

// Mogelijks:
// static int GetHour() {
//    return (Get() % 86400) / 3600;
// }