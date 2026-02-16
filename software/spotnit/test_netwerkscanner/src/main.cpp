#include <Arduino.h>
#include <iostream>
#include <String>
#include <vector>
#include "WiFi.h"

using namespace std;

#include "test1_2_3.h"

//---------------------------------
class AccesPoint
{
private:
  float x = 0.0f;
  float y = 0.0f;
  std::vector<int> rssi;
  std::vector<String> ssid;

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
  float GetAverageRssi()
  {
    int vectorSize = rssi.size();
    if (rssi.size() == 0)
      return 0.0f;

    int sum = 0;
    for (int i = 0; i < vectorSize; i++)
    {
      sum += rssi[i];
    }
    return sum / (float)vectorSize;
  }
};
int filter()
{ // kalman-filter

  return 0;
};

int findAP(String mac, int aantalNetwerken)
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

void Trilateratie()
{
  int aantalNetwerken = 0;
  do
  {
    aantalNetwerken = WiFi.scanNetworks(false, true, true, 100);
  } while (aantalNetwerken < 3);

  Serial.println("found networks " + String(aantalNetwerken));

  int net1 = findAP("A8:BA:25:50:A5:90", aantalNetwerken);
  int net2 = findAP("A8:BA:25:50:26:70", aantalNetwerken);
  int net3 = findAP("A8:BA:25:50:55:E0", aantalNetwerken);

  Serial.println("net1= " + String(net1));
  Serial.println("net2= " + String(net2));
  Serial.println("net3= " + String(net3));

  if (net1 == -1 || net2 == -1 || net3 == -1)
  {
    return;
  }

  AccesPoint accessPoint1(0, 0);
  AccesPoint accessPoint2(15, 0);
  AccesPoint accessPoint3(15, 9);
  AccesPoint device;

  float distance1 = RssiToMeter(WiFi.RSSI(net1));
  float distance2 = RssiToMeter(WiFi.RSSI(net2));
  float distance3 = RssiToMeter(WiFi.RSSI(net3));

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

void setup()
{
  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was previously
  // connected.
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false); // maakt RSSI meten veel nauwkeuriger
  WiFi.disconnect();
  delay(100);

  Serial.println("Setup done");
}

void loop()
{
  // test1_2();
  //   test3();
  Trilateratie();
}
