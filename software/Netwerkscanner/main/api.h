#ifndef API_H
#define API_H

#include "esp_wifi.h"
char* MakeWifiJsonExample();
void PrintApInfo(const wifi_ap_record_t* ap);
char* CreatWifiJson(wifi_ap_record_t* aps, uint16_t start, uint16_t count,
                    uint64_t TimeStart, uint64_t TimeEnd, int32_t x, int32_t y);
#endif