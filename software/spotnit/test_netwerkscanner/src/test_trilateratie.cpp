#include <Arduino.h>
#include <WiFi.h>
#include <iostream>
#include <String>
#include <vector>

#include "debug.h"

class AccesPoint
{
private:
    float x = 0.0f;
    float y = 0.0f;
    String macPrefix = ""; // eerste 10 hex waarden van max
    std::vector<int> RSSI;
    std::vector<String> SSID;
    std::vector<String> BSSID;

public:
    // Constructor
    AccesPoint(float xVal = 0.0f, float yVal = 0.0f) : x(xVal), y(yVal) {}

    // Getters
    float GetX() const
    {
        return x;
    }

    float GetY() const
    {
        return y;
    }
    String GetMacPrefix()
    {
        return macPrefix;
    }

    // Setters
    void SetX(float xVal)
    {
        x = xVal;
    }

    void SetY(float yVal)
    {
        y = yVal;
    }

    void SetPos(float xVal, float yVal)
    {
        x = xVal;
        y = yVal;
    }

    void AddRSSI(int rssi)
    {
        RSSI.push_back(rssi);
    }
    void AddSSID(String ssid)
    {
        SSID.push_back(ssid);
    }
    void AddBSSID(String bssid)
    {
        BSSID.push_back(bssid);
    }

    // funties
    float GetAverageRssi()
    {
        int vectorSize = RSSI.size();
        if (vectorSize == 0)
        {
            debugln("vector rssi was leeg");
            return 0.0f;
        }

        int sum = 0;
        for (int i = 0; i < vectorSize; i++)
        {
            sum += RSSI[i];
        }
        return sum / (float)vectorSize;
    }

    void ResetVectors()
    {
        RSSI.clear();
        SSID.clear();
        BSSID.clear();
    }
};

int findAPIndex(String mac, int aantalNetwerken)
{
    mac = mac.substring(0, mac.length() - 2);
    for (int i = 0; i < aantalNetwerken; ++i)
    {
        String MAC = WiFi.BSSIDstr(i);
        if (MAC.indexOf(mac) >= 0)
        {
            return i;
        }
    }
    return -1;
}

float RssiToMeter(int rssi)
{
    const int rssiOp1Meter = -40;      // rssi waarde op 1 meter
    const int propagationConstant = 3; // hoe goed kan het signaal in de ruimte bewegen.
    return powf(10, (rssiOp1Meter - rssi) / (10.0f * propagationConstant));
}
void Trilateratie() // heb het gevoel dat dit niet werkt.
{
    int aantalNetwerken = 0;
    do
    {
        aantalNetwerken = WiFi.scanNetworks(false, true, true, 100);
    } while (aantalNetwerken < 3);

    Serial.println("found networks " + String(aantalNetwerken));

    int net1 = findAPIndex("EA:9F:6D:88:2A:71", aantalNetwerken);
    int net2 = findAPIndex("EA:9F:6D:88:25:C1", aantalNetwerken);
    int net3 = findAPIndex("EA:9F:6D:88:F5:6D", aantalNetwerken);

    Serial.println("net1= " + String(net1) + " " + String(WiFi.RSSI(net1)));
    Serial.println("net2= " + String(net2) + " " + String(WiFi.RSSI(net2)));
    Serial.println("net3= " + String(net3) + " " + String(WiFi.RSSI(net3)));

    if (net1 == -1 || net2 == -1 || net3 == -1)
    {
        return;
    }

    AccesPoint accessPoint1(0, 4);
    AccesPoint accessPoint2(4, 0);
    AccesPoint accessPoint3(0, 0);
    AccesPoint device;

    float distance1 = -5; // RssiToMeter(WiFi.RSSI(net1));
    float distance2 = -5;   // RssiToMeter(WiFi.RSSI(net2));
    float distance3 = -5; // RssiToMeter(WiFi.RSSI(net3));

    Serial.println("afstand 1 = " + String(distance1));
    Serial.println("afstand 2 = " + String(distance2));
    Serial.println("afstand 3 = " + String(distance3));

    float A = 2 * (accessPoint1.GetX() - accessPoint2.GetX());
    float B = 2 * (accessPoint1.GetY() - accessPoint2.GetY());
    float C = distance2 * distance2 - distance1 * distance1 + accessPoint2.GetX() * accessPoint2.GetX() - accessPoint1.GetX() * accessPoint1.GetX() + accessPoint2.GetY() * accessPoint2.GetY() - accessPoint1.GetY() * accessPoint1.GetY();

    float D = 2 * (accessPoint1.GetX() - accessPoint3.GetX());
    float E = 2 * (accessPoint1.GetY() - accessPoint3.GetY());
    float F = distance3 * distance3 - distance1 * distance1 + accessPoint3.GetX() * accessPoint3.GetX() - accessPoint1.GetX() * accessPoint1.GetX() + accessPoint3.GetY() * accessPoint3.GetY() - accessPoint1.GetY() * accessPoint1.GetY();

    float denominator = (A * E - B * D);

    device.SetX((C * E - B * F) / denominator);
    device.SetY((A * F - C * D) / denominator);

    Serial.print("device X: ");
    Serial.println(device.GetX());

    Serial.print("device Y: ");
    Serial.println(device.GetY());
    WiFi.scanDelete();
}