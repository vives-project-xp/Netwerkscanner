#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED_GPIO GPIO_NUM_15   // vaak ingebouwde LED op ESP32 boards

void app_main(void)
{
    printf("hallo van esp\n");
    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    while (1) {
        gpio_set_level(LED_GPIO, 1);   // LED aan
        vTaskDelay(pdMS_TO_TICKS(100));

        gpio_set_level(LED_GPIO, 0);   // LED uit
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}