#include "esp_clk_tree.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#include <Arduino.h>
#include "screen.cpp"
#include "Test.h"
void CheckCpuFreq()
{
    uint32_t freq_hz;
    // Get the frequency of the CPU clock
    esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_CPU, ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT, &freq_hz);
    printf("CPU Clock: %lu Hz\n", freq_hz);
}
void print_ap_info(const wifi_ap_record_t *ap)
{
    printf("SSID: %s\n", ap->ssid);
    printf("BSSID: %02X:%02X:%02X:%02X:%02X:%02X\n",
           ap->bssid[0], ap->bssid[1], ap->bssid[2],
           ap->bssid[3], ap->bssid[4], ap->bssid[5]);
    printf("Primary Channel: %d\n", ap->primary);
    printf("Secondary Channel: %d\n", ap->second);
    printf("RSSI: %d\n", ap->rssi);
    printf("Auth Mode: %d\n", ap->authmode);
    printf("Pairwise Cipher: %d\n", ap->pairwise_cipher);
    printf("Group Cipher: %d\n", ap->group_cipher);
    printf("Antenna: %d\n", ap->ant);
    printf("PHY Modes: 11b=%d, 11g=%d, 11n=%d, 11a=%d, 11ac=%d, 11ax=%d, LR=%d\n",
           ap->phy_11b, ap->phy_11g, ap->phy_11n, ap->phy_11a, ap->phy_11ac, ap->phy_11ax, ap->phy_lr);
    printf("WPS: %d, FTM Responder: %d, FTM Initiator: %d\n",
           ap->wps, ap->ftm_responder, ap->ftm_initiator);
    printf("Bandwidth: %d, VHT freq1: %d, VHT freq2: %d\n",
           ap->bandwidth, ap->vht_ch_freq1, ap->vht_ch_freq2);
    // Country info
    printf("Country: %c%c%c\n", ap->country.cc[0], ap->country.cc[1], ap->country.cc[2]);
    printf("------------------------------------------------\n");
}
void ScanNetworks()
{
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

    for (int i = 0; i < 10; i++)
    {
        print_ap_info(&aps[i]); // pass the address of each element
    }
}
extern "C" void app_main(void)
{
    initArduino();
    printf("Hello World!\n");
    printf("Build Date: %s\n", __DATE__);
    printf("Build Time: %s\n", __TIME__);

    ScreenTest();

    vTaskDelay(pdMS_TO_TICKS(5000));
    Serial.begin(112500);
    vTaskDelay(pdMS_TO_TICKS(1000));
    Serial.println("Arduino");
    fflush(stdout);
    pinMode(15, OUTPUT);   // many ESP32 boards use GPIO2 for LED

    while (true)
    {
        digitalWrite(15, HIGH);
        delay(100);

        digitalWrite(15, LOW);
        delay(100);
    }

    CheckCpuFreq();
    ScanNetworks();

    // testDrawPixel();

    MyLed led;

    while (true)
    {
        led.on();

        led.off();
    }
}