#include "driver/gpio.h"
#include "esp_clk_tree.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_ota_ops.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "host/ble_hs.h"
#include "lwip/sockets.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "nvs_flash.h"
#include "services/gap/ble_svc_gap.h"

// Moet hier staan anders compile error door combinatie van
// lwIP en Arduino
#ifdef INADDR_NONE
#undef INADDR_NONE
#endif

#ifdef INADDR_ANY
#undef INADDR_ANY
#endif

// #include <Arduino.h>
#include "Test.h"
#include "WiFi.h"
#include "api.h"
#include "http_post.h"
#include "ota_server.h"
#include "screen.h"
#include "simple_fingerprinting.h"
#include "wifi_key.h"
#include "main.h"

#define ENABLE_CPU_MONITOR 1
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

static const char* LOGTAG = "MAIN.CPP";

#define GPIO_3V3 GPIO_NUM_0

#define DEBOUNCE_DELAY 60
#define GPIO_BUTTON_UP GPIO_NUM_2      // A1
#define GPIO_BUTTON_DOWN GPIO_NUM_4    // A3
#define GPIO_BUTTON_SELECT GPIO_NUM_5  // A4
#define GPIO_BUTTON_BACK GPIO_NUM_9    // SDA
#define GPIO_BUTTON_MULTI GPIO_NUM_10  // SDL
static volatile uint64_t lastIsrTimeUp = 0;
static volatile uint64_t lastIsrTimeDown = 0;
static volatile uint64_t lastIsrTimeSelect = 0;
static volatile uint64_t lastIsrTimeBack = 0;
static volatile uint64_t lastIsrTimeMulti = 0;
static volatile bool pressedUp = 0;
static volatile bool pressedDown = 0;
static volatile bool pressedSelect = 0;
static volatile bool pressedBack = 0;
static volatile bool pressedMulti = 0;

//typedef enum {
//  BUTTON_UP,
//  BUTTON_DOWN,
//  BUTTON_SELECT,
//  BUTTON_BACK,
//  BUTTON_MULTI,
//  EVENT_WIFI_CONNECTED,
//  EVENT_WIFI_DISCONNECTED,
//  EVENT_LOCATION
//} ButtonEventT;

typedef struct {
  wifi_ap_record_t* records;
  uint16_t count;
  uint64_t timeStart;
  uint64_t timeEnd;
} scanWifiResults_t;

typedef struct {
  char mac[18];
  uint16_t rssi;
  uint64_t timeDetection;
  uint16_t scanId;
} scanBluethoothResults_t;

typedef struct {
  bool wifi2_4Ghz;
  bool wifi5Ghz;
  bool bluetooth;
} ScanConfig_t;

ScanConfig_t GlobalScanConfig = {false, false, false};

       QueueHandle_t menuQueue = NULL;
static QueueHandle_t wifiQueue;
static QueueHandle_t BluetoothQueue;
static QueueSetHandle_t combinedQueueSet;

TaskHandle_t xScannerHandle = NULL;
TaskHandle_t xMonitorCpuHandle = NULL;

#define SERVER_IP "10.20.10.24"
#define SERVER_PORT 8081
const char* serverUrl = "http://10.20.10.24:8081/upload";
#define WIFI_CONNECTED_BIT BIT0    // verbondenMetWifi
#define SERVER_CONNECTED_BIT BIT1  // verbondenMetServer

static EventGroupHandle_t communicationStateGroup;

typedef struct {
  const char* ssid;
  const char* password;
} WifiCredentials_t;
static const WifiCredentials_t myWifiNetworks[] = {
    {WIFI_SSID_0, WIFI_PASSWORD_0},  // in wifi_key.h
    {WIFI_SSID_1, WIFI_PASSWORD_1},
};
#define MAX_NETWORKS (sizeof(myWifiNetworks) / sizeof(myWifiNetworks[0]))
static int currentMyWifiNetworksIndex = 0;

wifi_country_t countryBe = {
    .cc = "BE",          // Landcode
    .schan = 1,          // Startkanaal (meestal 1)
    .nchan = 13,         // Aantal kanalen voor 2.4GHz (1 t/m 13)
    .max_tx_power = 20,  // Maximaal zendvermogen (in dBm)
    .policy = WIFI_COUNTRY_POLICY_AUTO,  // Gebruik instellingen van verbonden
                                         // AP of lokaal
                                         //.wifi_5g_channel_mask
};

LocationBasket_t LocationBasket; 
SemaphoreHandle_t LocationMutex;

void ScanNetworks() {
  nvs_flash_init();
  esp_netif_init();
  esp_event_loop_create_default();
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_mode(WIFI_MODE_STA);
  esp_wifi_start();

  esp_wifi_scan_start(NULL, true);

  uint16_t n = 10;
  wifi_ap_record_t aps[10];
  esp_wifi_scan_get_ap_records(&n, aps);

  for (int i = 0; i < 10; i++) {
    PrintApInfo(&aps[i]);
  }
  return;
}
void CheckCpuFreq() {
  uint32_t freq_hz;
  // Get the frequency of the CPU clock
  esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_CPU,
                               ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT, &freq_hz);
  printf("CPU Clock: %lu Hz\n", freq_hz);
}
const char* Str(const char* prefix, int8_t val) {
  static char buf[32];  // static zorgt dat de tekst blijft bestaan na de return
  snprintf(buf, sizeof(buf), "%s%d", prefix, val);
  return buf;
}
void GetMyIp(char* ipAddress) {
  esp_netif_ip_info_t ipInfo;
  esp_netif_t* netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");

  if (netif != NULL && esp_netif_get_ip_info(netif, &ipInfo) == ESP_OK) {
    esp_ip4addr_ntoa(&ipInfo.ip, ipAddress, 16);
  } else {
    strcpy(ipAddress, "0.0.0.0");
  }
}
void PrintConfiguredChannels(const wifi_scan_config_t* ScanConfig) {
  printf("\n--- Actieve Scan Kanalen ---\n");

  printf("2.4GHz: ");
  bool found24 = false;
  for (int i = 1; i <= 14; i++) {
    if (ScanConfig->channel_bitmap.ghz_2_channels & (1 << i)) {
      printf("[%d] ", BIT_NUMBER_TO_CHANNEL(i, WIFI_BAND_2G));
      found24 = true;
    }
  }
  if (!found24) printf("Geen");

  printf("\n5GHz:   ");
  bool found5 = false;
  for (int i = 0; i <= 32; i++) {
    if (ScanConfig->channel_bitmap.ghz_5_channels & (1ULL << i)) {
      printf("[%d] ", BIT_NUMBER_TO_CHANNEL(i, WIFI_BAND_5G));
      found5 = true;
    }
  }
  if (!found5) printf("Geen");
  printf("\n---------------------------\n");
}
void MonitorCpuTask(void* pvParameters) {
  char buffer[1024];
  while (1) {
    printf("\n--- [CPU GEBRUIK] (uS = Tijd actief | %% = Belasting) ---\n");
    vTaskGetRunTimeStats(buffer);
    printf("%s\n", buffer);

    printf("--- [TAAK STATUS] (X=Run, R=Ready, B=Blocked, S=Suspended) ---\n");
    printf("Naam\t\tStat\tPrio\tVrijeStack\tID\n");
    vTaskList(buffer);
    printf("%s\n", buffer);

    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

void PrintFreeStackSize() {
  ESP_LOGI("Free Stack", "Taak: %s | Vrij: %u bytes", pcTaskGetName(NULL),
           (unsigned int)uxTaskGetStackHighWaterMark(NULL));
}

static void BluethoothHostTask(void* param) {
  // NimBLE host stack begint hier te draaien
  nimble_port_run();
  nimble_port_freertos_deinit();
}
static int ble_gap_event_handler(struct ble_gap_event* event, void* arg) {
  switch (event->type) {
    case BLE_GAP_EVENT_DISC:
      printf(
          "BLE Device Found: RSSI %d | Addr: %02x:%02x:%02x:%02x:%02x:%02x "
          "rssi %02x \n",
          event->disc.rssi, event->disc.addr.val[5], event->disc.addr.val[4],
          event->disc.addr.val[3], event->disc.addr.val[2],
          event->disc.addr.val[1], event->disc.addr.val[0], event->disc.rssi);

      scanBluethoothResults_t result;
      snprintf(result.mac, sizeof(result.mac), "%02x:%02x:%02x:%02x:%02x:%02x",
               event->disc.addr.val[5], event->disc.addr.val[4],
               event->disc.addr.val[3], event->disc.addr.val[2],
               event->disc.addr.val[1], event->disc.addr.val[0]);
      result.rssi = event->disc.rssi;
      result.scanId = 0;
      result.timeDetection = esp_timer_get_time();

      if (xQueueSend(BluetoothQueue, &result, pdMS_TO_TICKS(100)) != pdPASS) {
        ESP_LOGE("scan task", "Queue vol!");
      }

      break;

    case BLE_GAP_EVENT_DISC_COMPLETE:
      ESP_LOGI("BLE", "Scan complete");
      break;
  }
  return 0;
}
void StartBleScan(uint32_t duration_ms) {
  struct ble_gap_disc_params scan_params;

  // Default scan configuration
  scan_params.filter_duplicates = 1;
  scan_params.passive = 1;
  scan_params.itvl = 0;
  scan_params.window = 0;
  scan_params.filter_policy = BLE_HCI_SCAN_FILT_NO_WL;
  scan_params.limited = 0;

  // Ensure the host is synced before scanning
  if (!ble_hs_synced()) {
    ESP_LOGE("BLE", "Cannot scan: Host not synced");
    return;
  }

  int rc = ble_gap_disc(BLE_OWN_ADDR_PUBLIC, duration_ms, &scan_params,
                        ble_gap_event_handler, NULL);
  if (rc != 0) {
    ESP_LOGE("BLE", "Error starting scan; rc=%d", rc);
  }
}

void TryConnectToWifiAndServer(void* pvParameters) {
  bool lastWifiState = false;
  communicationStateGroup = xEventGroupCreate();

  // Wi-Fi configuratie
  wifi_config_t wifi_config = {};
  strcpy((char*)wifi_config.sta.ssid, WIFI_SSID_1);
  strcpy((char*)wifi_config.sta.password, WIFI_PASSWORD_1);
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  esp_wifi_connect();

  while (1) {
    esp_netif_ip_info_t ip_info;
    esp_netif_t* netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");

    if (esp_netif_get_ip_info(netif, &ip_info) == ESP_OK &&
        ip_info.ip.addr != 0) {
      xEventGroupSetBits(communicationStateGroup, WIFI_CONNECTED_BIT);
      if (lastWifiState == false) {
        ButtonEventT event = EVENT_WIFI_CONNECTED;
        lastWifiState = true;
        xQueueSend(menuQueue, &event, portMAX_DELAY);
      }

      // Stap 2: Probeer de server te "pingen" via een TCP socket
      struct sockaddr_in dest_addr;
      dest_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
      dest_addr.sin_family = AF_INET;
      dest_addr.sin_port = htons(SERVER_PORT);

      int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
      if (sock < 0) {
        ESP_LOGE("SERVER", "Socket aanmaken mislukt");
      } else {
        // Zet een timeout op de verbinding (2 seconden)
        struct timeval tv = {.tv_sec = 2, .tv_usec = 0};
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

        int err =
            connect(sock, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
        if (err == 0) {
          ESP_LOGI("SERVER", "Server bereikbaar (Ping OK)");
          xEventGroupSetBits(communicationStateGroup, SERVER_CONNECTED_BIT);
        } else {
          ESP_LOGW("SERVER", "Server niet bereikbaar");
          xEventGroupClearBits(communicationStateGroup, SERVER_CONNECTED_BIT);
        }
        close(sock);
        ButtonEventT event = EVENT_WIFI_CONNECTED;
        lastWifiState = true;
        xQueueSend(menuQueue, &event, portMAX_DELAY);
      }
    } else {
      // Geen Wi-Fi
      xEventGroupClearBits(communicationStateGroup,
                           WIFI_CONNECTED_BIT | SERVER_CONNECTED_BIT);

      if (lastWifiState == true) {
        ButtonEventT event = EVENT_WIFI_DISCONNECTED;
        lastWifiState = false;
        xQueueSend(menuQueue, &event, portMAX_DELAY);
      }

      // Switch to the next SSID in the list
      currentMyWifiNetworksIndex =
          (currentMyWifiNetworksIndex + 1) % MAX_NETWORKS;

      wifi_config_t wifi_config = {};
      strncpy((char*)wifi_config.sta.ssid,
              myWifiNetworks[currentMyWifiNetworksIndex].ssid, 32);
      strncpy((char*)wifi_config.sta.password,
              myWifiNetworks[currentMyWifiNetworksIndex].password, 64);

      ESP_LOGI("WIFI", "Switching to: %s",
               myWifiNetworks[currentMyWifiNetworksIndex].ssid);

      esp_wifi_disconnect();
      esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
      esp_wifi_connect();
    }
    vTaskDelay(pdMS_TO_TICKS(10000));  // Probeer elke 10 seconden opnieuw
  }
}

void ScannerTask(void* pvParameters) {
  while (1) {
    wifi_scan_config_t scanConfig = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = 0,
        .show_hidden = false,
        .scan_type = WIFI_SCAN_TYPE_ACTIVE,
        //.scan_type = WIFI_SCAN_TYPE_PASSIVE,
        .scan_time = {.active = {.min = 0, .max = 100}},
        .coex_background_scan = false};

    if (GlobalScanConfig.wifi2_4Ghz && !GlobalScanConfig.wifi5Ghz) {
      scanConfig.channel_bitmap.ghz_2_channels = 0x3ffe;
      scanConfig.channel_bitmap.ghz_5_channels = 0;
      printf("2.4\n");
    } else if (!GlobalScanConfig.wifi2_4Ghz && GlobalScanConfig.wifi5Ghz) {
      scanConfig.channel_bitmap.ghz_2_channels = 0;
      scanConfig.channel_bitmap.ghz_5_channels = 0xfeffffe;
      printf("5\n");
    } else if (GlobalScanConfig.wifi2_4Ghz && GlobalScanConfig.wifi5Ghz) {
      scanConfig.channel_bitmap.ghz_2_channels = 0x3ffe;
      scanConfig.channel_bitmap.ghz_5_channels = 0xfeffffe;
      printf("alles\n");
    }
    // bitmap scannen werkt wel maar het toont ook andere kanalen als die als
    // secondary of primary staan.

    // wifi scannen
    if (GlobalScanConfig.wifi2_4Ghz || GlobalScanConfig.wifi5Ghz) {
      PrintConfiguredChannels(&scanConfig);
      printf("free heap: %ld\n", esp_get_free_heap_size());

      uint64_t timeStart = esp_timer_get_time();
      esp_err_t ret = esp_wifi_scan_start(&scanConfig, true);
      if (ret == ESP_ERR_WIFI_STATE) {
        ESP_LOGE("SCAN", "Wifi status onjuist, is hij wel geinitialiseerd?");
      } else if (ret == ESP_ERR_NO_MEM) {
        ESP_LOGE("SCAN", "GEEN GEHEUGEN!  heap bezet.");
      } else if (ret != ESP_OK) {
        ESP_LOGE("SCAN", "Scan fout: %s", esp_err_to_name(ret));
      }
      uint64_t timeEnd = esp_timer_get_time();
      printf("scan klaar\n");

      uint16_t apCount = 0;
      esp_wifi_scan_get_ap_num(&apCount);
      if (apCount == 0) {
        ESP_LOGI("scanner task: ", "Geen netwerken gevonden.");
        continue;
      }

      ESP_LOGI("scanner task: ", "Aantal gevonden: %d", apCount);

      wifi_ap_record_t* apRecords =
          (wifi_ap_record_t*)malloc(sizeof(wifi_ap_record_t) * apCount);

      if (apRecords == NULL) {
        ESP_LOGE("scanner task", "Malloc faalde! Te weinig geheugen.");
        continue;
      }
      // lees uit wifi geheugen.
      if (esp_wifi_scan_get_ap_records(&apCount, apRecords) == ESP_OK) {
        ESP_LOGI("scan task", "Gevonden netwerken: %d", apCount);

        scanWifiResults_t results;
        results.records = apRecords;
        results.count = apCount;
        results.timeStart = timeStart;
        results.timeEnd = timeEnd;

        printf("scan time: %lld\n", (timeEnd - timeStart));

        if (xQueueSend(wifiQueue, &results, pdMS_TO_TICKS(100)) != pdPASS) {
          ESP_LOGE("scan task", "Queue vol! Data verwijderen.");
          free(apRecords);
        }
      } else {
        ESP_LOGE("scan task", "get_ap_records failed.\n");
        free(apRecords);  // Altijd free-en als get_records faalt
      }
    }

    // bluethooth
    if (GlobalScanConfig.bluetooth) {
      ESP_LOGI("SCAN", "Starting BLE Scan...");
      StartBleScan(5000);  // Scan for 5 seconds
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void JsonBuilderTask(void* pvParameters) {
  scanWifiResults_t wifiData;
  scanBluethoothResults_t btData;
  const int networkPostCount = 10;
  const int bluethoothPostCount = 10;
  QueueSetMemberHandle_t activeQueue;

  while (1) {
    activeQueue = xQueueSelectFromSet(combinedQueueSet, portMAX_DELAY);

    if (activeQueue == wifiQueue) {
      if (xQueueReceive(wifiQueue, &wifiData, 0) == pdPASS) {
        // Verwerk WiFi resultaten
        ESP_LOGI("JSON", "Bezig met verwerken van %d APs", wifiData.count);

        for (int i = 0; i < wifiData.count / networkPostCount; i++) {
          // stuur networkPostCount per json, de laatste zal de rest ook
          // verzenden.
          // er kunnen dus meer dan 10 netwerken toekomen in de json
          // TODO toekomst automatisch verdelen zodat met de minst aantal posts
          // het meest kan verstuurd worden
          uint8_t start = i * networkPostCount;
          uint8_t count = networkPostCount;
          uint8_t verschil = (wifiData.count - (start + count));
          if (verschil < 10) {
            count = count + verschil;
          }

          char* payload =
              CreatWifiJson(wifiData.records, start, count, wifiData.timeStart,
                            wifiData.timeEnd, 0, 0);
          int8_t respons;
          if (payload != NULL) {
            respons = SendJsonPost(payload, serverUrl);
            if (respons == 0) {
              ESP_LOGI("JSON", "Data verzonden naar server.");
            } else {
              ESP_LOGI("JSON", "Data niet verzonden naar server.");
            }

            free(payload);
          } else {
            ESP_LOGI("json builder task", "payload was empty");
          }
        }

        free(wifiData.records);
      }
    }

    else if (activeQueue == BluetoothQueue) {
      // Verwerk Bluetooth resultaten
      if (uxQueueMessagesWaiting(BluetoothQueue) >= bluethoothPostCount) {
        // Haal ze er alle 10 uit in een loop
        
        printf("Batch van 10 Bluetooth resultaten verwerkt!\n");
      }
    } else {
      UBaseType_t waiting = uxQueueMessagesWaiting(BluetoothQueue);

      for (int i = 0; i < waiting; i++) {
        xQueueReceive(BluetoothQueue, &btData, 0);
        // Voeg toe aan verzendlijst...
      }

    }
  }
}
void DrawMenu(const char* menuOptions[], int8_t menuIndex) {
  int option1 = 0x0000;
  int option2 = 0x0000;
  int option3 = 0x0000;
  int option4 = 0x0000;

  if (menuIndex == 1) {
    option1 = 0x000f;
  } else if (menuIndex == 2) {
    option2 = 0x000f;
  } else if (menuIndex == 3) {
    option3 = 0x000f;
  } else if (menuIndex == 4) {
    option4 = 0x000f;
  }

  DrawStringFast(0, 50, menuOptions[0], 0xffff, option1, 3);
  DrawStringFast(0, 80, menuOptions[1], 0xffff, option2, 3);
  DrawStringFast(0, 110, menuOptions[2], 0xffff, option3, 3);
  DrawStringFast(0, 140, menuOptions[3], 0xffff, option4, 3);

  int square1 = 0x001F;
  int square2 = 0x001F;
  int square3 = 0x001F;
  int square4 = 0x001F;

  // crasht niet door "Lazy Evaluation"
  if (GlobalScanConfig.wifi2_4Ghz) {
    square1 = 0x07E0;
  }
  if (GlobalScanConfig.wifi5Ghz) {
    square2 = 0x07E0;
  }
  if (GlobalScanConfig.bluetooth) {
    square3 = 0x07E0;
  }
  if (xMonitorCpuHandle) {
    square4 = 0x07E0;
  }

  DrawRectFilled(200, 55, 15, 15, 0x738E, square1, 2);
  DrawRectFilled(200, 85, 15, 15, 0x738E, square2, 2);
  DrawRectFilled(200, 115, 15, 15, 0x738E, square3, 2);
  DrawRectFilled(200, 145, 15, 15, 0x738E, square4, 2);
}

void ToggleSelect(int8_t menuIndex) {
  if (menuIndex == 4) {
    if (xMonitorCpuHandle == NULL) {
      xTaskCreate(MonitorCpuTask, "MonitorCpuTask", 4096, NULL, 5,
                  &xMonitorCpuHandle);
    } else {
      vTaskDelete(xMonitorCpuHandle);
      xMonitorCpuHandle = NULL;
    }
    return;
  }

  if (xScannerHandle == NULL) {
    xTaskCreate(ScannerTask, "ScannerTask", 4096, NULL, 5, &xScannerHandle);
  }
  // WIFI 2.4Ghz
  if (menuIndex == 1) {
    if (GlobalScanConfig.wifi2_4Ghz) {
      GlobalScanConfig.wifi2_4Ghz = false;
    } else {
      GlobalScanConfig.wifi2_4Ghz = true;
    }
  }

  // WIFI 5Ghz
  else if (menuIndex == 2) {
    if (GlobalScanConfig.wifi5Ghz) {
      GlobalScanConfig.wifi5Ghz = false;
    } else {
      GlobalScanConfig.wifi5Ghz = true;
    }
  }

  // BLUETOOTH
  else if (menuIndex == 3) {
    if (GlobalScanConfig.bluetooth) {
      GlobalScanConfig.bluetooth = false;
    } else {
      GlobalScanConfig.bluetooth = true;
    }
  }
}

void MenuTask(void* pvParameters) {
  ButtonEventT ontvangenEvent;
  int8_t menuIndex = 1;
  const char* menuOptions[] = {"Wifi 2.4 ", "Wifi 5   ", "Bluetooth",
                               "Debug RTOS"};
  uint8_t menuLength = ARRAY_SIZE(menuOptions);

  // start screen
  St7789Init();
  FillScreen(0x0000);
  BacklightOn();

  DrawWifiIcon(20, 20, 0xF800);
  printf("nog geen verbinding met wifi\n");

  DrawMenu(menuOptions, menuIndex);

  while (1) {
    // Wacht in ruststand tot er IETS in de brievenbus komt
    if (xQueueReceive(menuQueue, &ontvangenEvent, portMAX_DELAY)) {
      printf("melding brief ontvangen %lld\n", esp_timer_get_time());
      if (ontvangenEvent == BUTTON_UP) {
        menuIndex--;
        if (menuIndex < 1) {
          menuIndex = menuLength;
        }
        printf("Cursor omhoog naar: %d  %lld\n", menuIndex,
               esp_timer_get_time());
        DrawStringFast(132, 0, Str("UP   ", menuIndex), 0xffff, 0x0001, 3);
      } else if (ontvangenEvent == BUTTON_DOWN) {
        menuIndex++;
        if (menuIndex > menuLength) {
          menuIndex = 1;
        }
        printf("Cursor omlaag naar: %d\n", menuIndex);
        DrawStringFast(132, 0, Str("Down ", menuIndex), 0xffff, 0x0001, 3);
      } else if (ontvangenEvent == BUTTON_SELECT) {
        printf("GEKOZEN: Je hebt item %d geselecteerd!\n", menuIndex);
        DrawStringFast(132, 0, "select", 0xffff, 0x0001, 3);
        ToggleSelect(menuIndex);
      } else if (ontvangenEvent == BUTTON_BACK) {
        printf("back:\n");
        DrawStringFast(132, 0, "back   ", 0xffff, 0x0001, 3);
      } else if (ontvangenEvent == BUTTON_MULTI) {
        printf("Multi:\n");
        DrawStringFast(132, 0, "Multi   ", 0xffff, 0x0001, 3);
        esp_restart();
      } else if (ontvangenEvent == EVENT_LOCATION) {
        printf("Location:\n");
        if (xSemaphoreTake(LocationMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
          DrawStringFast(10, 220, Str("X ",LocationBasket.locationX), 0xffff, 0x0001, 3);
          DrawStringFast(10, 220, Str("Y ",LocationBasket.locationY), 0xffff, 0x0001, 3);
          xSemaphoreGive(LocationMutex);
        }
        
        
      } else if (ontvangenEvent == EVENT_WIFI_CONNECTED ||
                 ontvangenEvent == EVENT_WIFI_DISCONNECTED) {
        // Check de status zonder de taak te blokkeren
        EventBits_t status = xEventGroupGetBits(communicationStateGroup);

        if (status & SERVER_CONNECTED_BIT) {
          DrawWifiIcon(20, 20, 0x07E0);
          printf("verbonden met server\n");
        } else if (status & WIFI_CONNECTED_BIT) {
          DrawWifiIcon(20, 20, 0xFFE0);
          wifi_config_t conf;
          esp_wifi_get_config(WIFI_IF_STA, &conf);
          printf("wifi connected to: SSID: %s\n", (char*)conf.sta.ssid);

          char ip[16];
          GetMyIp(ip);
          DrawStringFast(40, 6, (char*)conf.sta.ssid, 0xffff, 0x0000, 1);
          DrawStringFast(40, 16, ip, 0xffff, 0x0000, 1);

        } else {
          DrawWifiIcon(20, 20, 0xF800);
          printf("geen verbinding met wifi\n");
        }
      }
      DrawMenu(menuOptions, menuIndex);
    }
  }
}

static void IRAM_ATTR buttonIsrUp(void* arg) {
  uint64_t now = esp_timer_get_time();

  // 1. Harde debounce: negeer alles binnen 50ms (50000 us)
  if (now - lastIsrTimeUp < 50000) {
    return;
  }

  // 2. Omdat we geen digitalRead doen, vertrouwen we op de 'state'
  if (!pressedUp) {
    // We gaan ervan uit dat dit de RISING edge is
    ButtonEventT event = BUTTON_UP;
    xQueueSendFromISR(menuQueue, &event, NULL);

    pressedUp = true;
    lastIsrTimeUp = now;
  } else {
    // We gaan ervan uit dat dit de FALLING edge is
    pressedUp = false;
    lastIsrTimeUp = now;
  }
}
static void IRAM_ATTR buttonIsrDown(void* arg) {
  uint64_t now = esp_timer_get_time();

  // 1. Harde debounce: negeer alles binnen 50ms (50000 us)
  if (now - lastIsrTimeDown < 50000) {
    return;
  }

  // 2. Omdat we geen digitalRead doen, vertrouwen we op de 'state'
  if (!pressedDown) {
    // We gaan ervan uit dat dit de RISING edge is
    ButtonEventT event = BUTTON_DOWN;
    xQueueSendFromISR(menuQueue, &event, NULL);

    pressedDown = true;
    lastIsrTimeDown = now;
  } else {
    // We gaan ervan uit dat dit de FALLING edge is
    pressedDown = false;
    lastIsrTimeDown = now;
  }
}
static void IRAM_ATTR buttonIsrSelect(void* arg) {
  uint64_t now = esp_timer_get_time();

  // 1. Harde debounce: negeer alles binnen 50ms (50000 us)
  if (now - lastIsrTimeSelect < 50000) {
    return;
  }

  // 2. Omdat we geen digitalRead doen, vertrouwen we op de 'state'
  if (!pressedSelect) {
    // We gaan ervan uit dat dit de RISING edge is
    ButtonEventT event = BUTTON_SELECT;
    xQueueSendFromISR(menuQueue, &event, NULL);

    pressedSelect = true;
    lastIsrTimeSelect = now;
  } else {
    // We gaan ervan uit dat dit de FALLING edge is
    pressedSelect = false;
    lastIsrTimeSelect = now;
  }
}
static void IRAM_ATTR buttonIsrBack(void* arg) {
  uint64_t now = esp_timer_get_time();

  // 1. Harde debounce: negeer alles binnen 50ms (50000 us)
  if (now - lastIsrTimeBack < 50000) {
    return;
  }

  // 2. Omdat we geen digitalRead doen, vertrouwen we op de 'state'
  if (!pressedBack) {
    // We gaan ervan uit dat dit de RISING edge is
    ButtonEventT event = BUTTON_BACK;
    xQueueSendFromISR(menuQueue, &event, NULL);

    pressedBack = true;
    lastIsrTimeBack = now;
  } else {
    // We gaan ervan uit dat dit de FALLING edge is
    pressedBack = false;
    lastIsrTimeBack = now;
  }
}
static void IRAM_ATTR buttonIsrMulti(void* arg) {
  uint64_t now = esp_timer_get_time();

  // 1. Harde debounce: negeer alles binnen 50ms (50000 us)
  if (now - lastIsrTimeMulti < 50000) {
    return;
  }

  // 2. Omdat we geen digitalRead doen, vertrouwen we op de 'state'
  if (!pressedMulti) {
    // We gaan ervan uit dat dit de RISING edge is
    ButtonEventT event = BUTTON_MULTI;
    xQueueSendFromISR(menuQueue, &event, NULL);

    pressedMulti = true;
    lastIsrTimeMulti = now;
  } else {
    // We gaan ervan uit dat dit de FALLING edge is
    pressedMulti = false;
    lastIsrTimeMulti = now;
  }
}

esp_err_t InitWifiBluethooth(void) {
  // 1. NVS (Nodig voor opslag van kalibratiedata van beide radio's)
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // 2. Wi-Fi Stack Setup (Basic Station Mode)
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

  esp_err_t responsCountry = esp_wifi_set_country(&countryBe);
  if (responsCountry != ESP_OK) {
    printf("Fout bij instellen landcode: %s\n",
           esp_err_to_name(responsCountry));
  }

  ESP_ERROR_CHECK(esp_wifi_start());

  // 3. NimBLE Stack Setup
  ESP_ERROR_CHECK(nimble_port_init());

  // Stel de GAP service in (verplicht voor een werkende stack)
  ble_svc_gap_init();
  ESP_ERROR_CHECK(ble_svc_gap_device_name_set("ESP32C5_SCANNER"));

  // Start de NimBLE achtergrond-taak (FreeRTOS wrapper)
  nimble_port_freertos_init(BluethoothHostTask);

  return ESP_OK;
}

void CreatButtonInterrupts() {
  

  gpio_config_t io_conf = {};
  io_conf.intr_type = GPIO_INTR_ANYEDGE;
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pin_bit_mask = (1ULL << GPIO_BUTTON_UP) | (1ULL << GPIO_BUTTON_DOWN) |
                         (1ULL << GPIO_BUTTON_SELECT) |
                         (1ULL << GPIO_BUTTON_BACK) |
                         (1ULL << GPIO_BUTTON_MULTI);
  io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
  gpio_config(&io_conf);

  gpio_install_isr_service(0);

  gpio_isr_handler_add(GPIO_BUTTON_UP, buttonIsrUp, NULL);
  gpio_isr_handler_add(GPIO_BUTTON_DOWN, buttonIsrDown, NULL);
  gpio_isr_handler_add(GPIO_BUTTON_SELECT, buttonIsrSelect, NULL);
  gpio_isr_handler_add(GPIO_BUTTON_BACK, buttonIsrBack, NULL);
  gpio_isr_handler_add(GPIO_BUTTON_MULTI, buttonIsrMulti, NULL);
}

void GpioSetup() {
  GpioScreenSetup();
  CreatButtonInterrupts();
  gpio_set_direction(GPIO_3V3, GPIO_MODE_OUTPUT);
  gpio_set_level(GPIO_3V3, 1);
}

extern "C" void app_main(void) {
  // initArduino();
  printf("Hello World!\n");
  printf("Build Date: %s\n", __DATE__);
  printf("Build Time: %s\n", __TIME__);

  LocationMutex = xSemaphoreCreateMutex();
  menuQueue = xQueueCreate(15, sizeof(ButtonEventT));
  if (menuQueue == NULL) {
        ESP_LOGE("INIT", "Fatal: Could not create menuQueue!");
        return;
    }
  GpioSetup();

  esp_err_t status = InitWifiBluethooth();
  if (status != ESP_OK) {
    ESP_LOGE(LOGTAG, "Radio initialisatie mislukt!");
    return;
  }
  
  wifiQueue = xQueueCreate(5, sizeof(scanWifiResults_t));
  BluetoothQueue = xQueueCreate(50, sizeof(scanBluethoothResults_t));

  combinedQueueSet = xQueueCreateSet(5 + 50);
  xQueueAddToSet(wifiQueue, combinedQueueSet);
  xQueueAddToSet(BluetoothQueue, combinedQueueSet);

  xTaskCreate(MenuTask,    // Naam van de functie
              "MenuTask",  // Naam voor debugging
              3096,        // Stack size in bytes
              NULL,        // Parameter die je mee wilt geven (optioneel)
              5,           // Prioriteit (hoger getal = hogere prioriteit)
              NULL  // Task handle (optioneel, om de task later aan te sturen)
  );
  xTaskCreate(TryConnectToWifiAndServer, "TryConnectToWifiAndServer", 2596,
              NULL, 5,
              NULL  //
  );
  // ota upload is sneller dan usb
  xTaskCreate(OtaWebserverTask, "ota server", 4096, NULL, 20, NULL);

  xTaskCreate(JsonBuilderTask, "json builder", 4096, NULL, 5, NULL);

  CheckCpuFreq();
}