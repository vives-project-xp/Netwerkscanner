#include "Test.h"

MyLed::MyLed()
{
    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
}

void MyLed::on()
{
    gpio_set_level(LED_GPIO, 1); // LED on
    vTaskDelay(pdMS_TO_TICKS(1000));
    printf("on\n");
}

void MyLed::off()
{
    gpio_set_level(LED_GPIO, 0); // LED off
    vTaskDelay(pdMS_TO_TICKS(1000));
    printf("off\n");
}