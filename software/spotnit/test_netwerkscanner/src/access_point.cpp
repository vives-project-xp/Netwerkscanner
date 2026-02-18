#include <Arduino.h>
#include <vector>
#include <String>

#include "debug.h"
#include "access_point.h"

// Constructor
AccessPoint::AccessPoint() {}
AccessPoint::AccessPoint(float xVal, float yVal, String macPrefix, float RMSE)
{
    x = xVal;
    y = yVal;
    z = 0.0f;
    this->macPrefix = macPrefix;
    this->RMSE = RMSE;
}

// Getters
float AccessPoint::GetX() const
{
    return x;
}

float AccessPoint::GetY() const
{
    return y;
}

float AccessPoint::GetZ() const
{
    return z;
}

float AccessPoint::GetRMSE() const
{
    return RMSE;
}

String AccessPoint::GetMacPrefix() const
{
    return macPrefix;
}

// Setters
void AccessPoint::SetX(float xVal)
{
    x = xVal;
}

void AccessPoint::SetY(float yVal)
{
    y = yVal;
}

void AccessPoint::SetPos(float xVal, float yVal)
{
    x = xVal;
    y = yVal;
}

void AccessPoint::SetRMSE(float rmse)
{
    RMSE = rmse;
}

void AccessPoint::AddRSSI(int rssi)
{
    RSSI.push_back(rssi);
}

void AccessPoint::AddSSID(String ssid)
{
    SSID.push_back(ssid);
}

void AccessPoint::AddBSSID(String bssid)
{
    BSSID.push_back(bssid);
}

// Functies
float AccessPoint::GetAverageRssi() const
{
    size_t vectorSize = RSSI.size();
    if (vectorSize == 0)
    {
        debugln("vector rssi was leeg");
        return -1;
    }

    int sum = 0;
    for (int val : RSSI)
    {
        sum += val;
    }
    return (float)sum / vectorSize;
}

void AccessPoint::ResetVectors()
{
    RSSI.clear();
    SSID.clear();
    BSSID.clear();
}