#include <Arduino.h>
#include <WiFi.h>
#include <iostream>
#include <String>
#include <vector>

#include "debug.h"

#include "access_point.h"


float gemX = 0;
float gemY = 0;



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
    const int rssiOp1Meter = -34;      // rssi waarde op 1 meter
    const float propagationConstant = 2.9; // hoe goed kan het signaal in de ruimte bewegen.
    return powf(10, (rssiOp1Meter - rssi) / (10.0f * propagationConstant));
}
void Trilateratie() // heb het gevoel dat dit niet werkt.
{
    int aantalNetwerken = 0;
    do
    {
        aantalNetwerken = WiFi.scanNetworks(false, true, true, 100, 1);
    } while (aantalNetwerken < 3);

    //Serial.println("found networks " + String(aantalNetwerken));

    int net1 = findAPIndex("EA:9F:6D:88:2A:71", aantalNetwerken);
    int net2 = findAPIndex("EA:9F:6D:88:25:C1", aantalNetwerken);
    int net3 = findAPIndex("EA:9F:6D:88:F5:6D", aantalNetwerken);

    //Serial.println("net1= " + String(net1) + " " + String(WiFi.RSSI(net1)));
    //Serial.println("net2= " + String(net2) + " " + String(WiFi.RSSI(net2)));
    //Serial.println("net3= " + String(net3) + " " + String(WiFi.RSSI(net3)));

    if (net1 == -1 || net2 == -1 || net3 == -1)
    {
        return;
    }

    AccessPoint accessPoint1(0, 4);
    AccessPoint accessPoint2(0, 0);
    AccessPoint accessPoint3(-2, 4);
    AccessPoint device;

    float distance1 = RssiToMeter(WiFi.RSSI(net1));
    float distance2 = RssiToMeter(WiFi.RSSI(net2));
    float distance3 = RssiToMeter(WiFi.RSSI(net3));

    //Serial.println("afstand 1 = " + String(distance1));
    //Serial.println("afstand 2 = " + String(distance2));
    //Serial.println("afstand 3 = " + String(distance3));

    float A = 2 * (accessPoint1.GetX() - accessPoint2.GetX());
    float B = 2 * (accessPoint1.GetY() - accessPoint2.GetY());
    float C = distance2 * distance2 - distance1 * distance1 + accessPoint2.GetX() * accessPoint2.GetX() - accessPoint1.GetX() * accessPoint1.GetX() + accessPoint2.GetY() * accessPoint2.GetY() - accessPoint1.GetY() * accessPoint1.GetY();

    float D = 2 * (accessPoint1.GetX() - accessPoint3.GetX());
    float E = 2 * (accessPoint1.GetY() - accessPoint3.GetY());
    float F = distance3 * distance3 - distance1 * distance1 + accessPoint3.GetX() * accessPoint3.GetX() - accessPoint1.GetX() * accessPoint1.GetX() + accessPoint3.GetY() * accessPoint3.GetY() - accessPoint1.GetY() * accessPoint1.GetY();

    float denominator = (A * E - B * D);

    device.SetX((C * E - B * F) / denominator);
    device.SetY((A * F - C * D) / denominator);

    //Serial.print("device X: ");
    //Serial.println(device.GetX());

    //Serial.print("device Y: ");
    //Serial.println(device.GetY());

    gemX = (gemX + device.GetX()) / 2.0;
    gemY = (gemY + device.GetY()) / 2.0;

    debugln(String(gemX) + " " + String(gemY));

    WiFi.scanDelete();
}

