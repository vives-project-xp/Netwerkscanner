#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_ota_ops.h"

static const char* TAG = "OTA_SERVER";

const char* uploadHtml =
    "<html><body style=\"font-family: sans-serif;\">"
    "<head>"
    "<meta charset=\"UTF-8\">"
    "</head>"
    "<h1>NetwerkScanner</h1>"
    "<form method=\"POST\" action=\"/update\" enctype=\"multipart/form-data\">"
    "<input type=\"file\" name=\"update\" accept=\".bin\">"
    "<input type=\"submit\" value=\"Update Starten\">"
    "</form>"
    "<p>Upload NetwerkScanner.bin\n</p>"
    "<p>software\\NetwerkScanner\\build\\NetwerkScanner.bin\n</p>"
    "<pre style=\"color: #0f0; background-color: #000; padding: 15px; "
    "font-weight: bold; display: inline-block; border-radius: 5px; "
    "font-family: 'Courier New', Courier, monospace, 'Apple Color Emoji', "
    "'Segoe UI Emoji';\">"
    "     /\\ /|\n"
    "    (  °w°)\n"
    "    ( >&#128225; )\n"
    "     || ||\n"
    " [ SCANNING... ]"
    "</pre>"
    "</body></html>";

const char* successHtml =
    "<html>"
    "<head><meta charset=\"UTF-8\"><meta http-equiv=\"refresh\" "
    "content=\"5;url=/\"></head>"
    "<body style=\"font-family: sans-serif;\">"
    "<h1>Update succesvol!</h1>"
    "<p>Het apparaat herstart... Je wordt over 5 seconden teruggestuurd.</p>"
    "<pre style=\"color: #0f0; background-color: #000; padding: 15px; "
    "font-weight: bold; display: inline-block; border-radius: 5px; "
    "font-family: 'Courier New', Courier, monospace, 'Apple Color Emoji', "
    "'Segoe UI Emoji';\">"
    "      /\\ /|\n"
    "     (  °w°)\n"
    "     ( >&#128225; )\n"
    "      || ||\n"
    " [ REBOOTING... ]"
    "</pre>"
    "</body>"
    "</html>";

/* GET Handler: Toont de upload pagina */
esp_err_t UploadGetHandler(httpd_req_t* req) {
  httpd_resp_send(req, uploadHtml, HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

/* POST Handler: Ontvangt de .bin file en schrijft naar OTA partitie */
esp_err_t UploadPostHandler(httpd_req_t* req) {
  char buf[1024];
  esp_ota_handle_t updateHandle = 0;
  const esp_partition_t* updatePartition =
      esp_ota_get_next_update_partition(NULL);

  int remaining = req->content_len;
  bool isOtaBegun = false;
  esp_err_t err;

  while (remaining > 0) {
    int recvLen = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)));
    if (recvLen <= 0) {
      if (recvLen == HTTPD_SOCK_ERR_TIMEOUT) continue;
      return ESP_FAIL;
    }

    // --- DE CRUCIALE STAP: Overslaan van de HTTP Multipart Header ---
    int dataOffset = 0;
    if (!isOtaBegun) {
      // We zoeken naar de start van de echte binary (0xE9)
      for (int i = 0; i < recvLen; i++) {
        if ((uint8_t)buf[i] == 0xE9) {
          dataOffset = i;
          isOtaBegun = true;
          ESP_LOGI(TAG, "Magic byte gevonden op offset %d. OTA Start!", i);
          break;
        }
      }
      if (!isOtaBegun) {
        remaining -= recvLen;
        continue;  // Nog geen binary gevonden in dit blok
      }

      err = esp_ota_begin(updatePartition, OTA_SIZE_UNKNOWN, &updateHandle);
      if (err != ESP_OK) return ESP_FAIL;
    }
    // ---------------------------------------------------------------

    err = esp_ota_write(updateHandle, &buf[dataOffset], recvLen - dataOffset);
    if (err != ESP_OK) {
      esp_ota_abort(updateHandle);
      return ESP_FAIL;
    }

    remaining -= recvLen;
  }

  // Afronden
  esp_ota_end(updateHandle);
  esp_ota_set_boot_partition(updatePartition);

  httpd_resp_sendstr(req, successHtml);
  vTaskDelay(pdMS_TO_TICKS(2000));
  esp_restart();
  return ESP_OK;
}

void OtaWebserverTask(void* pvParameters) {
  httpd_handle_t server = NULL;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.max_uri_handlers = 2;

  ESP_LOGI(TAG, "Server starten op poort: '%d'", config.server_port);
  if (httpd_start(&server, &config) == ESP_OK) {
    // GET route
    httpd_uri_t getUri = {.uri = "/",
                          .method = HTTP_GET,
                          .handler = UploadGetHandler,
                          .user_ctx = NULL};
    httpd_register_uri_handler(server, &getUri);

    // POST route
    httpd_uri_t postUri = {.uri = "/update",
                           .method = HTTP_POST,
                           .handler = UploadPostHandler,
                           .user_ctx = NULL};
    httpd_register_uri_handler(server, &postUri);
  }

  // De taak blijft draaien om de server in de lucht te houden
  while (1) {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}