
#include "esp_clk_tree.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <Arduino.h>
#include "screen.h"
#include "Test.h"
#include "simple_fingerprinting.h"
#include "WiFi.h"
#include "http_post.h"
#include "api.h"
#include "http_post.h"
#include "wifi_key.h"

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
        PrintApInfo(&aps[i]);
    }
    return;
}
void CheckCpuFreq()
{
    uint32_t freq_hz;
    // Get the frequency of the CPU clock
    esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_CPU, ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT, &freq_hz);
    printf("CPU Clock: %lu Hz\n", freq_hz);
}
extern "C" void app_main(void)
{
    initArduino();
    printf("Hello World!\n");
    printf("Build Date: %s\n", __DATE__);
    printf("Build Time: %s\n", __TIME__);

    // 
    // ScreenTest();

    WiFi.begin(WIFI_SSID,WIFI_PASSWORD );
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    const char *serverUrl = "http://10.20.10.24:8081/upload";

    String payload(MakeJson());
    SendJsonPost(payload, serverUrl);

    printf("send data to server\n");
    MakeJson();

    ScanNetworks();

    // Serial.println("Arduino");
    // fflush(stdout);
    printf("123456789\n");
    // TestSimpleFingerprinting();

    CheckCpuFreq();

    // testDrawPixel();

    MyLed led;

    while (true)
    {
        led.on();

        led.off();
    }
}