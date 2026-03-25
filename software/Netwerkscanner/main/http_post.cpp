#include <WiFi.h>
#include <HTTPClient.h>
#include "esp_log.h"

static const char *LOGTAG = "HTTP_POST.CPP";

void SendJsonPost(const String &payload, const char *serverUrl)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        ESP_LOGI(LOGTAG,"Not connected to wifi\n");
        return;
    }

    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverUrl);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0)
    {
        printf("Code: %d\n", httpResponseCode);

        String response = http.getString();
        printf("Server response: %s\n", response.c_str());
    }
    else
    {
        printf("Error: %s\n", http.errorToString(httpResponseCode).c_str());
    }

    http.end();
}