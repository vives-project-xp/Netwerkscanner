#include <Arduino.h>
#include <WiFiManager.h>

bool wifiConnection = false;
bool serverConnection = false;

void setup()
{
  Serial.begin(115200);

  //TODO add custom setting to wifiManager, (server ip, token,...)
  WiFiManager wifiManager;
  // wifiManager.resetSettings();
  wifiManager.setDebugOutput(true);
  wifiManager.setConfigPortalTimeout(180);
  wifiConnection = wifiManager.autoConnect("NetwerkscannerWifiSetup", "password");
  if (!wifiConnection)
  {
    Serial.println("Failed to connect");
    // ESP.restart();
  }
  else
  {
    Serial.println("connected...yeey :)");
  }
}

void loop()
{
}
