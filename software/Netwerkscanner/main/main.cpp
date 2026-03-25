
#include "esp_clk_tree.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"

#include <Arduino.h>
#include "screen.h"
#include "Test.h"
#include "simple_fingerprinting.h"
#include "WiFi.h"
#include "http_post.h"
#include "api.h"
#include "http_post.h"
#include "wifi_key.h"

static const char *LOGTAG = "MAIN.CPP";

#define DEBOUNCE_DELAY_US 100000ULL // 100ms
static volatile uint64_t last_isr_time = 0;
static volatile uint32_t counter = 0;
static QueueHandle_t button_queue;

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

void MenuTask(void *pvParameters)
{
    while (1)
    {
        ESP_LOGI(LOGTAG, "Hallo van task menu");

        // Wacht 1000ms (vTaskDelay verwacht ticks, pdMS_TO_TICKS rekent dit om)
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void IRAM_ATTR button_isr(void *arg)
{
    uint64_t now = esp_timer_get_time();
    if (now - last_isr_time > DEBOUNCE_DELAY_US)
    {
        counter = counter + 1;
        uint32_t cnt = counter;
        BaseType_t higher_priority_task_woken = pdFALSE;
        xQueueSendFromISR(button_queue, &cnt, &higher_priority_task_woken); // Send counter to queue from ISR
        last_isr_time = now;
        if (higher_priority_task_woken)
        {
            portYIELD_FROM_ISR();
        }
    }
}

extern "C" void app_main(void)
{
    initArduino();
    printf("Hello World!\n");
    printf("Build Date: %s\n", __DATE__);
    printf("Build Time: %s\n", __TIME__);

    button_queue = xQueueCreate(10, sizeof(uint32_t));

    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << GPIO_NUM_4);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_NUM_4, button_isr, NULL);

    uint32_t button_counter;

    // Keep program running
    while (1)
    {
        // Wait indefinitely for an item in the queue
        if (xQueueReceive(button_queue, &button_counter, portMAX_DELAY))
        {
            printf("Button pressed %lu times.\n", button_counter);
        }
    }

    xTaskCreate(
        MenuTask,   // Naam van de functie
        "MenuTask", // Naam voor debugging
        4096,       // Stack size in bytes
        NULL,       // Parameter die je mee wilt geven (optioneel)
        1,          // Prioriteit (hoger getal = hogere prioriteit)
        NULL        // Task handle (optioneel, om de task later aan te sturen)
    );

    //
    // ScreenTest();

    /*WiFi.begin(WIFI_SSID,WIFI_PASSWORD );

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        ESP_LOGI("wifi connection", "not connected");
    }*/
    const char *serverUrl = "http://10.20.10.24:8081/upload";

    String payload(MakeWifiJson());
    // SendJsonPost(payload, serverUrl);

    printf("send data to server\n");
    // MakeJson();

    // ScanNetworks();

    // TestSimpleFingerprinting();

    CheckCpuFreq();

    // testDrawPixel();
}