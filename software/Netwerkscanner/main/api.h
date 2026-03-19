#ifndef API_H
#define API_H


#include "esp_wifi.h"
void TestJson(wifi_ap_record_t *aps);
char* MakeJson();
void print_ap_info(const wifi_ap_record_t *ap);
char *CreateJson(wifi_ap_record_t *aps, int count, int64_t TimeStart, int64_t TimeEnd, int32_t x, int32_t y);
#endif