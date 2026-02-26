#ifndef ACCESS_POINT
#define ACCESS_POINT

#include <Arduino.h>
#include <vector>
#include <String>
#include "debug.h"

class AccessPoint
{
private:
    float x;
    float y;
    float z;
    float RMSE;       // root mean squared error
    String macPrefix; // eerste 10 hex waarden van MAC vb "EA:9F:6D:88:2A:71" -> "EA:9F:6D:88:2A"
    std::vector<int> RSSI;
    std::vector<String> SSID;
    std::vector<String> BSSID;

public:
    // Constructor
    AccessPoint();
    AccessPoint(float xVal, float yVal, String macPrefix = "", float RMSE = 0.0f);

    // Getters
    float GetX() const;
    float GetY() const;
    float GetZ() const;
    float GetRMSE() const;
    String GetMacPrefix() const;

    // Setters
    void SetX(float xVal);
    void SetY(float yVal);
    void SetPos(float xVal, float yVal);
    void SetRMSE(float rmse);

    void AddRSSI(int rssi);
    void AddSSID(String ssid);
    void AddBSSID(String bssid);

    // Functies
    float GetAverageRssi() const;
    float RssiToMeter(float rssi) const;
    void ResetVectors();
};

#endif