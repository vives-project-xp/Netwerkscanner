#include <Arduino.h>

/*
 *  ESP32 WiFi Scanner Example. Examples > WiFi > WiFiScan
 *  Full Tutorial @
 * https://deepbluembedded.com/esp32-wifi-library-examples-tutorial-arduino/
 */

#include "WiFi.h"

void test1_2() // vind alle netwerken.
{
  Serial.println("Scan start");

  // WiFi.scanNetworks will return the number of networks found.
  int n = WiFi.scanNetworks(false, true, false, 80);
  Serial.println("Scan done");
  if (n == 0)
  {
    Serial.println("no networks found");
  }
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    Serial.println(
        "Nr | SSID                             | RSSI |  | BSSID                    | CH | Encryption");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.printf("%2d", i + 1);
      Serial.print(" | ");

      Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
      Serial.print(" | ");

      Serial.printf("%4d", WiFi.RSSI(i));
      Serial.print(" | ");

      Serial.print(" | BSSID: ");
      Serial.print(WiFi.BSSIDstr(i));
      Serial.print(" | ");

      Serial.printf("%2d", WiFi.channel(i));
      Serial.print(" | ");

      switch (WiFi.encryptionType(i))
      {
      case WIFI_AUTH_OPEN:
        Serial.print("open");
        break;
      case WIFI_AUTH_WEP:
        Serial.print("WEP");
        break;
      case WIFI_AUTH_WPA_PSK:
        Serial.print("WPA");
        break;
      case WIFI_AUTH_WPA2_PSK:
        Serial.print("WPA2");
        break;
      case WIFI_AUTH_WPA_WPA2_PSK:
        Serial.print("WPA+WPA2");
        break;
      case WIFI_AUTH_WPA2_ENTERPRISE:
        Serial.print("WPA2-EAP");
        break;
      case WIFI_AUTH_WPA3_PSK:
        Serial.print("WPA3");
        break;
      case WIFI_AUTH_WPA2_WPA3_PSK:
        Serial.print("WPA2+WPA3");
        break;
      case WIFI_AUTH_WAPI_PSK:
        Serial.print("WAPI");
        break;
      default:
        Serial.print("unknown");
      }
      Serial.println();
      delay(10);
    }
  }
  Serial.println("");

  // Delete the scan result to free memory for code below.
  WiFi.scanDelete();

  // Wait a bit before scanning again.
}

void test3() // snelheid test
{

  const int scans = 10;
  unsigned int timeStart = 0;
  unsigned int timeStop = 0;
  timeStart = millis();
  for (int i = 0; i < scans; i++)
  {
    WiFi.scanNetworks(false, true, true, 80); // online repo kiest ook 80ms per channel
  }
  timeStop = millis();
  Serial.println("gemiddelde voor 1 scan = " + String((timeStop - timeStart) / scans));
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
    aantalNetwerken = WiFi.scanNetworks(false, true, true, 80);
  } while (aantalNetwerken < 3);

  const float p1Start = RssiToMeter(WiFi.RSSI(0));
  const float p2Start = RssiToMeter(WiFi.RSSI(1));
  const float p3Start = RssiToMeter(WiFi.RSSI(2));

  float RelativePosition[2] = {0, 0};
  while (true)
  {
  }
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
  test1_2();
  // test3();
}
