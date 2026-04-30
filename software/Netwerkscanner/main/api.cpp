#include "api.h"

#include <stdio.h>
#include <stdlib.h>

#include <string>

#include "cJSON.h"
#include "esp_chip_info.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

static const char* LOGTAG = "API.CPP";

void PrintApInfo(const wifi_ap_record_t* ap)  // data van 1 ap
{
  printf("SSID: %s\n", ap->ssid);
  printf("BSSID: %02X:%02X:%02X:%02X:%02X:%02X\n", ap->bssid[0], ap->bssid[1],
         ap->bssid[2], ap->bssid[3], ap->bssid[4], ap->bssid[5]);
  printf("Primary Channel: %d\n", ap->primary);
  printf("Secondary Channel: %d\n", ap->second);
  printf("RSSI: %d\n", ap->rssi);
  printf("Auth Mode: %d\n", ap->authmode);
  printf("Pairwise Cipher: %d\n", ap->pairwise_cipher);
  printf("Group Cipher: %d\n", ap->group_cipher);
  printf("Antenna: %d\n", ap->ant);
  printf("PHY Modes: 11b=%d, 11g=%d, 11n=%d, 11a=%d, 11ac=%d, 11ax=%d, LR=%d\n",
         ap->phy_11b, ap->phy_11g, ap->phy_11n, ap->phy_11a, ap->phy_11ac,
         ap->phy_11ax, ap->phy_lr);
  printf("WPS: %d, FTM Responder: %d, FTM Initiator: %d\n", ap->wps,
         ap->ftm_responder, ap->ftm_initiator);
  printf("Bandwidth: %d, VHT freq1: %d, VHT freq2: %d\n", ap->bandwidth,
         ap->vht_ch_freq1, ap->vht_ch_freq2);
  // Country info
  printf("Country: %c%c%c\n", ap->country.cc[0], ap->country.cc[1],
         ap->country.cc[2]);
  printf("------------------------------------------------\n");
}

std::string GetChipName() {
  esp_chip_info_t info;
  esp_chip_info(&info);

  if (info.model == CHIP_ESP32) return "ESP32";
  if (info.model == CHIP_ESP32S2) return "ESP32-S2";
  if (info.model == CHIP_ESP32S3) return "ESP32-S3";
  if (info.model == CHIP_ESP32C3) return "ESP32-C3";
  if (info.model == CHIP_ESP32H2) return "ESP32-H2";
  if (info.model == CHIP_ESP32C2) return "ESP32-C2";
  if (info.model == CHIP_ESP32C5) return "ESP32-C5";
  return "???";
}
const char* GetChipId() {
  std::string chipName = GetChipName();
  uint8_t mac[6];
  esp_efuse_mac_get_default(mac);

  static char buffer[32];
  sprintf(buffer, "%s_%02X%02X", chipName.c_str(), mac[4], mac[5]);
  return buffer;
}

char* CreatWifiJson(wifi_ap_record_t* aps, uint16_t start, uint16_t count,
                    uint64_t TimeStart, uint64_t TimeEnd, int32_t x = 0,
                    int32_t y = 0) {
  // TODO tijdelijke fix
  if (count > 30) {
    count = 30;  // zorg er voor dat het niet crasht door teveel geheugen
  }

  cJSON* root = cJSON_CreateObject();

  if (root == NULL) {
    ESP_LOGI(LOGTAG, "Failed to create JSON root");
    return NULL;
  }

  cJSON_AddStringToObject(root, "device_id", GetChipId());
  cJSON_AddNumberToObject(root, "scan_time_start", TimeStart);
  cJSON_AddNumberToObject(root, "scan_time_end", TimeEnd);
  cJSON_AddNumberToObject(root, "x", x);
  cJSON_AddNumberToObject(root, "y", y);

  cJSON* networks = cJSON_CreateArray();
  for (uint16_t i = 0; i < count; i++) {
    cJSON* network1 = cJSON_CreateObject();
    cJSON_AddStringToObject(network1, "ssid", (const char*)aps[start + i].ssid);
    char bssid_str[18];
    sprintf(bssid_str, "%02X:%02X:%02X:%02X:%02X:%02X", aps[start + i].bssid[0],
            aps[start + i].bssid[1], aps[start + i].bssid[2],
            aps[start + i].bssid[3], aps[start + i].bssid[4],
            aps[start + i].bssid[5]);

    cJSON_AddStringToObject(network1, "bssid", bssid_str);
    cJSON_AddNumberToObject(network1, "primary_channel",
                            aps[start + i].primary);
    cJSON_AddNumberToObject(network1, "secondary_channel",
                            aps[start + i].second);
    cJSON_AddNumberToObject(network1, "rssi", aps[start + i].rssi);
    cJSON_AddNumberToObject(network1, "auth_mode", aps[start + i].authmode);
    cJSON_AddNumberToObject(network1, "pairwise_cipher",
                            aps[start + i].pairwise_cipher);
    cJSON_AddNumberToObject(network1, "group_cipher",
                            aps[start + i].group_cipher);
    cJSON_AddNumberToObject(network1, "antenna", aps[start + i].ant);
    cJSON_AddNumberToObject(network1, "phy_modes_11b", aps[start + i].phy_11a);
    cJSON_AddNumberToObject(network1, "phy_modes_11g", aps[start + i].phy_11g);
    cJSON_AddNumberToObject(network1, "phy_modes_11n", aps[start + i].phy_11n);
    cJSON_AddNumberToObject(network1, "phy_modes_11a", aps[start + i].phy_11a);
    cJSON_AddNumberToObject(network1, "phy_modes_11ac",
                            aps[start + i].phy_11ac);
    cJSON_AddNumberToObject(network1, "phy_modes_11ax",
                            aps[start + i].phy_11ax);
    cJSON_AddNumberToObject(network1, "phy_modes_lr", aps[start + i].phy_lr);
    cJSON_AddNumberToObject(network1, "wps", aps[start + i].wps);
    cJSON_AddNumberToObject(network1, "ftm_responder",
                            aps[start + i].ftm_responder);
    cJSON_AddNumberToObject(network1, "ftm_initiator",
                            aps[start + i].ftm_initiator);
    cJSON_AddNumberToObject(network1, "bandwidth", aps[start + i].bandwidth);
    cJSON_AddNumberToObject(network1, "vht_freq1", aps[start + i].vht_ch_freq1);
    cJSON_AddNumberToObject(network1, "vht_freq2", aps[start + i].vht_ch_freq2);
    cJSON_AddNumberToObject(
        network1, "he_ap_bss_color",
        aps[start + i].he_ap.bss_color); /**< The BSS Color value associated
                                      with the AP's corresponding BSS */
    cJSON_AddNumberToObject(
        network1, "he_ap_partial_bss_color",
        aps[start + i]
            .he_ap.bss_color); /**< Indicates whether an AID assignment rule
                            is based on the BSS color */
    cJSON_AddNumberToObject(
        network1, "he_ap_bss_color_disabled",
        aps[start + i].he_ap.bss_color_disabled); /**< Indicates whether the BSS
                                               color usage is disabled */
    cJSON_AddNumberToObject(
        network1, "he_ap_bssid_index",
        aps[start + i].he_ap.bssid_index); /**< In a M-BSSID set, identifies the
                                        non-transmitted BSSID */
    cJSON_AddNumberToObject(network1, "reserved", aps[start + i].reserved);
    char countryCode[4];
    countryCode[0] = aps[start + i].country.cc[0];
    countryCode[1] = aps[start + i].country.cc[1];
    countryCode[2] = aps[start + i].country.cc[2];
    countryCode[3] = '\0';
    cJSON_AddStringToObject(network1, "country", (const char*)countryCode);

    cJSON_AddItemToArray(networks, network1);
  }

  cJSON_AddItemToObject(root, "networks", networks);

  // char* json_string = cJSON_Print(root);             // pretty printing
  char* json_string = cJSON_PrintUnformatted(root);  // zonder pretty printing

  cJSON_Delete(root);
  // printf(json_string); fout veroorzaakt mogelijks crash
  // puts(json_string);// door het te verwijderen crasht het niet.
  //  daardoor kon ik zien dat json_string empty is.

  if (json_string != NULL) {
    puts(json_string);
  }
  return json_string;
}

double GetJsonNumber(const char *json_string, const char *key) {
    double result = 0;
    cJSON *root = cJSON_Parse(json_string);
    
    if (root != NULL) {
        cJSON *item = cJSON_GetObjectItemCaseSensitive(root, key);
        if (cJSON_IsNumber(item)) {
            result = item->valuedouble;
        }
        cJSON_Delete(root);
    }
    
    return result;
}

char* CreateBluethoothJson() {
  return "nog niet gemaakt";
}

char* MakeWifiJsonExample() {
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_mode(WIFI_MODE_STA);
  esp_wifi_start();

  esp_wifi_scan_start(NULL, true);
  uint16_t n = 10;
  wifi_ap_record_t aps[10];
  esp_wifi_scan_get_ap_records(&n, aps);

  int64_t startTime = esp_timer_get_time();
  int64_t endTime = esp_timer_get_time();

  ESP_LOGI(LOGTAG, "Creating JSON...");
  char* json_data = CreatWifiJson(aps, 0, n, startTime, endTime);

  if (json_data == NULL) {
    ESP_LOGI(LOGTAG, "JSON not Generated");
  } else {
    ESP_LOGI(LOGTAG, "Generated JSON:");
  }
  return json_data;
}

/*
API POST example

CreatWifiJson()
{
        "device_id":    "ESP32-C5_FFE8",
        "scan_time_start":      10675583,
        "scan_time_end":        10675585,
        "x":    0,
        "y":    0,
        "networks":     [{
                        "ssid": "Proximus-Home",
                        "bssid":        "68:12:C8:6C:F1:D2",
                        "primary_channel":      11,
                        "secondary_channel":    0,
                        "rssi": -82,
                        "auth_mode":    3,
                        "pairwise_cipher":      4,
                        "group_cipher": 4,
                        "antenna":      0,
                        "phy_modes_11b":        0,
                        "phy_modes_11g":        1,
                        "phy_modes_11n":        1,
                        "phy_modes_11a":        0,
                        "phy_modes_11ac":       0,
                        "phy_modes_11ax":       0,
                        "phy_modes_lr": 0,
                        "wps":  0,
                        "ftm_responder":        0,
                        "ftm_initiator":        0,
                        "bandwidth":    1,
                        "vht_freq1":    0,
                        "vht_freq2":    0,
                        "he_ap_bss_color":      0,
                        "he_ap_partial_bss_color":      0,
                        "he_ap_bss_color_disabled":     0,
                        "he_ap_bssid_index":    0,
                        "reserved":     0,
                        "country":      "BE "
                },{
                        "ssid": "Proximus-Home",
                        "bssid":        "68:12:C8:6C:F1:D3",
                        "primary_channel":      52,
                        "secondary_channel":    1,
                        "rssi": -91,
                        "auth_mode":    3,
                        "pairwise_cipher":      4,
                        "group_cipher": 4,
                        "antenna":      0,
                        "phy_modes_11b":        1,
                        "phy_modes_11g":        0,
                        "phy_modes_11n":        1,
                        "phy_modes_11a":        1,
                        "phy_modes_11ac":       1,
                        "phy_modes_11ax":       1,
                        "phy_modes_lr": 0,
                        "wps":  1,
                        "ftm_responder":        0,
                        "ftm_initiator":        0,
                        "bandwidth":    4,
                        "vht_freq1":    58,
                        "vht_freq2":    50,
                        "he_ap_bss_color":      26,
                        "he_ap_partial_bss_color":      26,
                        "he_ap_bss_color_disabled":     0,
                        "he_ap_bssid_index":    0,
                        "reserved":     0,
                        "country":      "BE\u0004"
                }]
}

*/