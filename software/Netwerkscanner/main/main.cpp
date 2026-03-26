
#include "esp_clk_tree.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "lwip/sockets.h"




#ifdef INADDR_NONE
#undef INADDR_NONE
#endif

#ifdef INADDR_ANY
#undef INADDR_ANY
#endif

//#include <Arduino.h>
#include "screen.h"
#include "Test.h"
#include "simple_fingerprinting.h"
#include "WiFi.h"
#include "http_post.h"
#include "api.h"
#include "http_post.h"
#include "wifi_key.h"
//#include "screen_lvgl.cpp"

static const char *LOGTAG = "MAIN.CPP";

#define GPIO_3V3 GPIO_NUM_0

#define DEBOUNCE_DELAY 100000ULL      // 100ms
#define GPIO_BUTTON_SELECT GPIO_NUM_5     // A4
#define GPIO_BUTTON_DOWN GPIO_NUM_4   // A3
#define GPIO_BUTTON_UP GPIO_NUM_2 // A1
#define GPIO_BUTTON_BACK GPIO_NUM_9   // SDA
#define GPIO_BUTTON_MULTI GPIO_NUM_10 // SDL
static volatile uint64_t lastIsrTimeUp = 0;
static volatile uint64_t lastIsrTimeDown = 0;
static volatile uint64_t lastIsrTimeSelect = 0;
static volatile uint64_t lastIsrTimeBack = 0;
static volatile uint64_t lastIsrTimeMulti = 0;

typedef enum
{
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_SELECT,
    BUTTON_BACK,
    BUTTON_MULTI,
    EVENT_WIFI_CONNECTED,
    EVENT_WIFI_DISCONNECTED
} ButtonEventT;

static QueueHandle_t menuQueue;
static QueueHandle_t wifi2_4GhzQueue;
static QueueHandle_t wifi5GhzQueue;
static QueueHandle_t BluetoothQueue;

#define SERVER_IP "10.20.10.24"
#define SERVER_PORT 8081
#define WIFI_CONNECTED_BIT BIT0   // verbondenMetWifi
#define SERVER_CONNECTED_BIT BIT1 // verbondenMetServer

static EventGroupHandle_t s_status_event_group;

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

static void BluethoothHostTask(void *param)
{
    // NimBLE host stack begint hier te draaien
    nimble_port_run();
    nimble_port_freertos_deinit();
}

void TryConnectToWifiAndServer(void *pvParameters)
{
    s_status_event_group = xEventGroupCreate();

    // Wi-Fi configuratie
    wifi_config_t wifi_config = {};
    strcpy((char *)wifi_config.sta.ssid, WIFI_SSID);
    strcpy((char *)wifi_config.sta.password, WIFI_PASSWORD);
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    esp_wifi_connect();

    while (1)
    {
        // Stap 1: Wacht tot Wi-Fi verbonden is (check elke 5 sec)
        esp_netif_ip_info_t ip_info;
        esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");

        if (esp_netif_get_ip_info(netif, &ip_info) == ESP_OK && ip_info.ip.addr != 0)
        {
            xEventGroupSetBits(s_status_event_group, WIFI_CONNECTED_BIT);

            // Stap 2: Probeer de server te "pingen" via een TCP socket
            struct sockaddr_in dest_addr;
            dest_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
            dest_addr.sin_family = AF_INET;
            dest_addr.sin_port = htons(SERVER_PORT);

            int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
            if (sock < 0)
            {
                ESP_LOGE("SERVER", "Socket aanmaken mislukt");
            }
            else
            {
                // Zet een timeout op de verbinding (2 seconden)
                struct timeval tv = {.tv_sec = 2, .tv_usec = 0};
                setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

                int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
                if (err == 0)
                {
                    ESP_LOGI("SERVER", "Server bereikbaar (Ping OK)");
                    xEventGroupSetBits(s_status_event_group, SERVER_CONNECTED_BIT);
                }
                else
                {
                    ESP_LOGW("SERVER", "Server niet bereikbaar");
                    xEventGroupClearBits(s_status_event_group, SERVER_CONNECTED_BIT);
                }
                close(sock);
            }
        }
        else
        {
            // Geen Wi-Fi
            xEventGroupClearBits(s_status_event_group, WIFI_CONNECTED_BIT | SERVER_CONNECTED_BIT);
            esp_wifi_connect();
        }

        vTaskDelay(pdMS_TO_TICKS(10000)); // Probeer elke 5 seconden opnieuw
    }
}

void Wifi2_4GhzTask(void *pvParameters)
{
    vTaskDelete(NULL);
}
void Wifi5GhzTask(void *pvParameters)
{
    vTaskDelete(NULL);
}
void BluetoothTask(void *pvParameters)
{
    vTaskDelete(NULL);
}

void MenuTask(void *pvParameters)
{
    ButtonEventT ontvangenEvent;
    int8_t menu_index = 0;

    // start screen
    BacklightOn();
    St7789Init();
    FillScreen(0xffff);

    while (1)
    {
        // Check de status zonder de taak te blokkeren
        EventBits_t status = xEventGroupGetBits(s_status_event_group);

        if (status & SERVER_CONNECTED_BIT)
        {
            FillScreen(0xF81F);
        }
        else if (status & WIFI_CONNECTED_BIT)
        {
            FillScreen(0x0000);
        }
        else
        {
            FillScreen(0xF800);
        }

        // Wacht in ruststand tot er IETS in de brievenbus komt
        if (xQueueReceive(menuQueue, &ontvangenEvent, portMAX_DELAY))
        {

            if (ontvangenEvent == BUTTON_UP)
            {
                menu_index++;
                printf("Cursor omhoog naar: %d\n", menu_index);
                FillScreen(0xff00);
            }
            else if (ontvangenEvent == BUTTON_DOWN)
            {
                menu_index--;
                printf("Cursor omlaag naar: %d\n", menu_index);
                FillScreen(0x00ff);
            }
            else if (ontvangenEvent == BUTTON_SELECT)
            {
                printf("GEKOZEN: Je hebt item %d geselecteerd!\n", menu_index);
                FillScreen(0x0000);
            }
            else if (ontvangenEvent == BUTTON_BACK)
            {
                printf("back: item %d geselecteerd!\n", menu_index);
                FillScreen(0xf0f0);
            }
            else if (ontvangenEvent == BUTTON_MULTI)
            {
                printf("Multi: item %d !\n", menu_index);
                FillScreen(0x0f0f);
            }
            else if (ontvangenEvent == EVENT_WIFI_CONNECTED)
            {
                printf("Multi: item %d !\n", menu_index);
                FillScreen(0x0f0f);
            }
            else if (ontvangenEvent == EVENT_WIFI_DISCONNECTED)
            {
                printf("Multi: item %d !\n", menu_index);
                FillScreen(0x0f0f);
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
static void IRAM_ATTR buttonIsrBack(void *arg)
{
    uint64_t now = esp_timer_get_time();
    if (now - lastIsrTimeBack > DEBOUNCE_DELAY)
    {
        ButtonEventT event = BUTTON_BACK;
        xQueueSendFromISR(menuQueue, &event, NULL);
        lastIsrTimeBack = now;
    }
}
static void IRAM_ATTR buttonIsrMulti(void *arg)
{
    uint64_t now = esp_timer_get_time();
    if (now - lastIsrTimeMulti > DEBOUNCE_DELAY)
    {
        ButtonEventT event = BUTTON_MULTI;
        xQueueSendFromISR(menuQueue, &event, NULL);
        lastIsrTimeMulti = now;
    }
}

esp_err_t InitWifiBluethooth(void)
{
    // 1. NVS (Nodig voor opslag van kalibratiedata van beide radio's)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
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

void CreatButtonInterrupts()
{

    menuQueue = xQueueCreate(10, sizeof(uint8_t));

    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_NEGEDGE; // falling edge
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << GPIO_BUTTON_UP) |
                           (1ULL << GPIO_BUTTON_DOWN) |
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

void GpioSetup()
{
    gpio_set_direction(GPIO_3V3, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_3V3, 1);
    GpioScreenSetup();
    CreatButtonInterrupts();
}

extern "C" void app_main(void)
{
    //initArduino();
    printf("Hello World!\n");
    printf("Build Date: %s\n", __DATE__);
    printf("Build Time: %s\n", __TIME__);

    GpioSetup();

    esp_err_t status = InitWifiBluethooth();
    if (status != ESP_OK)
    {
        ESP_LOGE(LOGTAG, "Radio initialisatie mislukt!");
        return;
    }

    xTaskCreate(
        MenuTask,   // Naam van de functie
        "MenuTask", // Naam voor debugging
        4096,       // Stack size in bytes
        NULL,       // Parameter die je mee wilt geven (optioneel)
        5,          // Prioriteit (hoger getal = hogere prioriteit)
        NULL        // Task handle (optioneel, om de task later aan te sturen)
    );
    xTaskCreate(
        TryConnectToWifiAndServer,
        "TryConnectToWifiAndServer",
        4096,
        NULL,
        7,
        NULL);

    xTaskCreate(
        Wifi2_4GhzTask,
        "Wifi2.4Task",
        4096,
        NULL,
        6,
        NULL);
    xTaskCreate(
        Wifi5GhzTask,
        "Wifi5GhzTask",
        4096,
        NULL,
        6,
        NULL);
    xTaskCreate(
        BluetoothTask,
        "BluetoothTask",
        4096,
        NULL,
        6,
        NULL);

    //
    // ScreenTest();


    const char *serverUrl = "http://10.20.10.24:8081/upload";

    String payload(MakeWifiJson());
    SendJsonPost(payload, serverUrl);

    printf("send data to server\n");
    // MakeJson();

    // ScanNetworks();

    // TestSimpleFingerprinting();

    CheckCpuFreq();

    // testDrawPixel();
}