#include <Arduino.h>
#include <WiFi.h>


void WifiScanningSetup(){
    WiFi.mode(WIFI_AP_STA);
    WiFi.setSleep(false);
    
}