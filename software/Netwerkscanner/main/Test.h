#ifndef MYLED_H
#define MYLED_H

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LED_GPIO GPIO_NUM_15   // change to your LED pin if needed

class MyLed
{
public:
    void on();
    void off();
};

#endif