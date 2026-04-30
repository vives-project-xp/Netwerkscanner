#ifndef MAIN_H
#define MAIN_H


extern QueueHandle_t menuQueue;

typedef enum {
  BUTTON_UP,
  BUTTON_DOWN,
  BUTTON_SELECT,
  BUTTON_BACK,
  BUTTON_MULTI,
  EVENT_WIFI_CONNECTED,
  EVENT_WIFI_DISCONNECTED,
  EVENT_LOCATION
} ButtonEventT;


typedef struct {
    float locationX;
    float locationY;
} LocationBasket_t;

extern LocationBasket_t LocationBasket;
extern SemaphoreHandle_t LocationMutex;

#endif