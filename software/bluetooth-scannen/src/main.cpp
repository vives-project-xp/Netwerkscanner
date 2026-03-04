#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

BLEScan* pBLEScan;

int scanTime = 5; // scan tijd in seconden

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        // print one JSON object per device (newline separated)
        String name = advertisedDevice.getName().c_str();
        String addr = advertisedDevice.getAddress().toString().c_str();
        int rssi = advertisedDevice.getRSSI();

        // escape quotes in name/address if needed (very simple)
        name.replace("\"","\\\"");
        addr.replace("\"","\\\"");

        Serial.print("{\"name\":\"");
        Serial.print(name);
        Serial.print("\",\"address\":\"");
        Serial.print(addr);
        Serial.print("\",\"rssi\":");
        Serial.print(rssi);
        Serial.println("}");
    }
};

void setup() {
    Serial.begin(115200);
    Serial.println("BLE Scanner gestart...");

    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true); // Actief scannen (meer data, iets meer stroom)
}

void loop() {
    Serial.println("Scannen...");
    // indicate start of scan so host can group results
    Serial.println("SCAN_START");

    BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
    Serial.print("Aantal apparaten gevonden: ");
    Serial.println(foundDevices.getCount());
    Serial.println("Scan klaar!\n");

    // mark end of this scan batch
    Serial.println("SCAN_END");

    delay(10000); // wacht 10 sec voor volgende scan
}