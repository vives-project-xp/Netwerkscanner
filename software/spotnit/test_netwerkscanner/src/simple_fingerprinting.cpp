#include <Arduino.h>
#include <WiFi.h>

#include "access_point.h"

class DataPoint
{
private:
    int x = 0;
    int y = 0;
    std::vector<int> rssi;
    std::vector<String> mac;

public:
    // --- Position ---
    int getX() const { return x; }
    int getY() const { return y; }

    void setPoint(int xValue, int yValue)
    {
        x = xValue;
        y = yValue;
    }

    // --- RSSI / MAC toevoegen ---
    void addMeasurement(int rssiValue, const String &macAddress)
    {
        rssi.push_back(rssiValue);
        mac.push_back(macAddress);
    }
    // --- Aantal metingen ---
    int getMeasurementCount() const
    {
        return rssi.size();
    }

    // --- Individuele meting ophalen ---
    int getRssi(int index) const
    {
        return rssi.at(index);
    }

    String getMac(int index) const
    {
        return mac.at(index);
    }
};

String Serial_ask(String question = "") // leest tot '\n' karakter
{
    while (Serial.available())// leeg buffer
    {
        Serial.read();
    }
    Serial.println(question);

    while (Serial.available() == 0)
    { // wait for data available
    }
    String line = Serial.readString();
    line.trim();
    Serial.println(line);

    return line;
}

std::vector<DataPoint> datapoints;
void Scan()
{
    DataPoint datapoint;
    int aantalNetwerken = 0;
    aantalNetwerken = WiFi.scanNetworks(false, true, true, 150);
    if (aantalNetwerken <= 0)
    {
        return;
    }

    int x = Serial_ask("Enter X cor").toInt();
    int y = Serial_ask("Enter Y cor").toInt();

    datapoint.setPoint(x, y);
    for (int i = 0; i < aantalNetwerken; i++)
    {
        datapoint.addMeasurement(WiFi.RSSI(i), WiFi.BSSIDstr(i));
    }
    datapoints.push_back(datapoint);
}

void printData()
{
    for (int i = 0; i < datapoints.size(); i++)
    {
        Serial.println("-------------------------------------------");
        Serial.println("meetpunt" + String(i));
        Serial.println("X = " + String(datapoints[i].getX()));
        Serial.println("Y = " + String(datapoints[i].getY()));
        for (int j = 0; j < datapoints[i].getMeasurementCount(); j++)
        {
            Serial.println(String(datapoints[i].getRssi(j)) + " - " + String(datapoints[i].getMac(j)));
        }
    }
}

void test_simple_fingerprinting()
{
    Scan();
    printData();
}