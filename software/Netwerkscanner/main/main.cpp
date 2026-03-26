
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

#define DEBOUNCE_DELAY 100000ULL      // 100ms
#define GPIO_BUTTON_UP GPIO_NUM_5     // A4
#define GPIO_BUTTON_DOWN GPIO_NUM_4   // A3
#define GPIO_BUTTON_SELECT GPIO_NUM_2 // A1
static volatile uint64_t lastIsrTimeUp = 0;
static volatile uint64_t lastIsrTimeDown = 0;
static volatile uint64_t lastIsrTimeSelect = 0;
static volatile uint8_t buttonCountUp = 0;
static volatile uint8_t buttonCountDown = 0;
static volatile uint8_t buttonCountSelect = 0;

typedef enum
{
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_SELECT
} ButtonEventT;

static QueueHandle_t menuQueue;

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
    ButtonEventT ontvangen_event;
    int8_t menu_index = 0;

    while (1)
    {
        // Wacht in ruststand tot er IETS in de brievenbus komt
        if (xQueueReceive(menuQueue, &ontvangen_event, portMAX_DELAY))
        {

            if (ontvangen_event == BUTTON_UP)
            {
                menu_index++;
                printf("Cursor omhoog naar: %d\n", menu_index);
            }
            else if (ontvangen_event == BUTTON_DOWN)
            {
                menu_index--;
                printf("Cursor omlaag naar: %d\n", menu_index);
            }
            else if (ontvangen_event == BUTTON_SELECT)
            {
                printf("GEKOZEN: Je hebt item %d geselecteerd!\n", menu_index);
                // Voer hier de actie uit die bij dit menu-item hoort
            }
        }
    }
}

static void IRAM_ATTR buttonIsrUp(void *arg)
{
    uint64_t now = esp_timer_get_time();
    if (now - lastIsrTimeUp > DEBOUNCE_DELAY)
    {
        ButtonEventT event = BUTTON_UP;
        xQueueSendFromISR(menuQueue, &event, NULL);
        lastIsrTimeUp = now;
    }
}

static void IRAM_ATTR buttonIsrDown(void *arg)
{
    uint64_t now = esp_timer_get_time();
    if (now - lastIsrTimeDown > DEBOUNCE_DELAY)
    {
        ButtonEventT event = BUTTON_DOWN;
        xQueueSendFromISR(menuQueue, &event, NULL);
        lastIsrTimeDown = now;
    }
}

static void IRAM_ATTR buttonIsrSelect(void *arg)
{
    uint64_t now = esp_timer_get_time();
    if (now - lastIsrTimeSelect > DEBOUNCE_DELAY)
    {
        ButtonEventT event = BUTTON_SELECT;
        xQueueSendFromISR(menuQueue, &event, NULL);
        lastIsrTimeSelect = now;
    }
}

void CreatButtonInterrupts()
{

    menuQueue = xQueueCreate(10, sizeof(uint8_t));

    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_NEGEDGE; // falling edge
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << GPIO_BUTTON_UP) |
                           (1ULL << GPIO_BUTTON_DOWN) |
                           (1ULL << GPIO_BUTTON_SELECT);
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    gpio_install_isr_service(0);

    gpio_isr_handler_add(GPIO_BUTTON_UP, buttonIsrUp, NULL);
    gpio_isr_handler_add(GPIO_BUTTON_DOWN, buttonIsrDown, NULL);
    gpio_isr_handler_add(GPIO_BUTTON_SELECT, buttonIsrSelect, NULL);
}

void GpioSetup()
{
    GpioScreenSetup();
    CreatButtonInterrupts();
}

extern "C" void app_main(void)
{
    initArduino();
    printf("Hello World!\n");
    printf("Build Date: %s\n", __DATE__);
    printf("Build Time: %s\n", __TIME__);

    GpioSetup();

    xTaskCreate(
        MenuTask,   // Naam van de functie
        "MenuTask", // Naam voor debugging
        4096,       // Stack size in bytes
        NULL,       // Parameter die je mee wilt geven (optioneel)
        5,          // Prioriteit (hoger getal = hogere prioriteit)
        NULL        // Task handle (optioneel, om de task later aan te sturen)
    );

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
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