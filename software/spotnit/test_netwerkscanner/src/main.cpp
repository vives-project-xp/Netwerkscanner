#include <Arduino.h>
#include <iostream>
#include <String>
#include <vector>
#include "WiFi.h"

using namespace std;

#include "debug.h"
bool debugStatus = true;

#include "test1_2_3.h"
#include "test_trilateratie.h"



//---------------------------------


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
  Trilateratie();
}

void loop()
{
  // test1_2();
  //   test3();
  // Trilateratie();
}
