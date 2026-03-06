#include <Arduino.h>
#include <iostream>
#include <String>
#include <vector>
#include "WiFi.h"

#include "debug.h"
bool debugStatus = true;

#include "test1_2_3.h"
#include "test_trilateratie.h"
#include "trilateratie_n_points.h"
#include "non_linear_least_squares.h"
#include "simple_fingerprinting.h"

void AddRSSItoMac(std::vector<AccessPoint> &aps, int aantalNetwerken) //  bij gevonden mac juiste rssi toevoegen.
{
  for (int i = 0; i < aantalNetwerken; i++)
  {
    String macAddress = WiFi.BSSIDstr(i);

    for (int j = 0; j < aps.size(); j++)
    {
      if (macAddress.indexOf(aps[j].GetMacPrefix()) >= 0)
      {
        aps[j].AddRSSI(WiFi.RSSI(i));
      }
    }
  }
}

void ScanNetworks()
{
  // maak aps
  std::vector<AccessPoint> aps = {
      {6, 0, "EA:9F:6D:88:2A"},
      {0, 0, "EA:9F:6D:88:25"},
      {0, -7, "EA:9F:6D:88:F5"},
      {3, -9, "A8:BA:25:50:A5"}};

  unsigned long vorigeMillis = millis();
  while (millis() - vorigeMillis < 2000) // blijf scannen voor 5 seconden
  {
    // scan wifi netwerken
    int aantalNetwerken = 0;
    aantalNetwerken = WiFi.scanNetworks(false, true, true, 100);
    // debugln(String(aantalNetwerken)+" aantal gevonden netwerken");
    //  bij gevonden mac juiste rssi toevoegen.
    AddRSSItoMac(aps, aantalNetwerken);
    // kalman filter
  }
  debugln("scannen klaar");

  // Trilateratie
  std::vector<double> distances = {};
  for (int i = 0; i < aps.size(); i++)
  {
    float averageRssi = aps[i].GetAverageRssi();
    if (averageRssi != -1)
    {
      distances.push_back(aps[i].RssiToMeter(averageRssi));
    }
    else
    {
      aps.erase(aps.begin() + i);
    }
  }

  AccessPoint device;
  TrilaterationResult resultaat = TrilateratieLeastSquares(aps, distances);
  TestNonLinearLeastSquares(aps, distances);
  // device.SetPos(resultaat.x, resultaat.y);
  // device.SetRMSE(resultaat.RMSE);
  debugln("test_TrilateratieLeastSquares");
  debugln("positie = " + String(device.GetX()) + " " + String(device.GetY()));
  debugln("RMSE " + String(device.GetRMSE()));
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
  Serial.println("hello world");
}

void loop()
{
  // Test1_2();
  // Test3();
  // Trilateratie();
  //  TrilateratieLeastSquares();
  // TestTrilateratieLeastSquares();
  // ScanNetworks();
  TestSimpleFingerprinting();
}
