#include <stdio.h>
#include <string.h>

#include "api.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "main.h"
// #include "freertos/semphr.h"

static const char* TAG = "HTTP_POST";

// Event handler om de response van de server te verwerken
esp_err_t _http_event_handler(esp_http_client_event_t* evt) {
  switch (evt->event_id) {
    case HTTP_EVENT_ON_DATA:
      if (!esp_http_client_is_chunked_response(evt->client)) {
        int len = evt->data_len;
        char buffer[256];
        if (len < sizeof(buffer)) {
          memcpy(buffer, evt->data, len);
          buffer[len] = '\0';  // Safe null-termination

          // 2. Parse the JSON from our local buffer
          float locationX = (float)GetJsonNumber(buffer, "x");
          float locationY = (float)GetJsonNumber(buffer, "y");

          ESP_LOGI("HTTP_CLIENT", "Ontvangen: %s", buffer);
          ESP_LOGI("HTTP_CLIENT", "Locatie: x=%.2f, y=%.2f", locationX,
                   locationY);

          // 3. Thread-safe storage and Queue notification
          if (LocationMutex != NULL &&
              xSemaphoreTake(LocationMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            LocationBasket.locationX = locationX;
            LocationBasket.locationY = locationY;
            xSemaphoreGive(LocationMutex);

            ButtonEventT event = EVENT_LOCATION;
            // Use xQueueSend instead of FromISR since this is a callback, not a
            // hardware ISR
            xQueueSend(menuQueue, &event, pdMS_TO_TICKS(10));
          }
        } else {
          ESP_LOGE("HTTP_CLIENT", "Response te groot voor buffer!");
        }
      }
      break;
    default:
      break;
  }
  return ESP_OK;
}

int8_t SendJsonPost(const char* payload, const char* serverUrl) {
  esp_http_client_config_t config = {};
  config.url = serverUrl;
  config.event_handler = _http_event_handler;
  config.method = HTTP_METHOD_POST;

  esp_http_client_handle_t client = esp_http_client_init(&config);

  // Headers en body instellen
  esp_http_client_set_header(client, "Content-Type", "application/json");
  esp_http_client_set_post_field(client, payload, strlen(payload));

  // De request uitvoeren
  esp_err_t err = esp_http_client_perform(client);

  if (err == ESP_OK) {
    int status_code = esp_http_client_get_status_code(client);
    long long content_length = esp_http_client_get_content_length(client);
    ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %lld", status_code,
             content_length);

    esp_http_client_cleanup(client);
    return 0;
  } else {
    ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    esp_http_client_cleanup(client);
    return 1;
  }

  // Geheugen netjes opruimen
}
