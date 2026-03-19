#pragma once// zorgt dat de header maar 1 keer geladen word ook al gebruik je het in meerdere .cpp
#include <Arduino.h>

extern bool debugStatus;  // wordt in main.cpp gedefinieerd

#define debugln(x) if (debugStatus) Serial.println(x)
#define debug(x)  if (debugStatus) Serial.print(x)

