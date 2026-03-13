#include "Test.h"

void MyLed::on()
{
    gpio_set_level(LED_GPIO, 1); // LED on
    vTaskDelay(pdMS_TO_TICKS(100));
}

void MyLed::off()
{
    gpio_set_level(LED_GPIO, 0); // LED off
    vTaskDelay(pdMS_TO_TICKS(100));
}