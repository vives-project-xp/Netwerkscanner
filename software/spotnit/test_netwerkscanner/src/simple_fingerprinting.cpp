#include <Arduino.h>
#include <WiFi.h>

#include "access_point.h"
#include "debug.h"

class DataPoint
{
public:
    float x = 0;
    float y = 0;
    float deviationSum = 0;
    float deviationCounter = 0;
    std::vector<int> rssi;
    std::vector<String> mac;

public:
    // --- Position ---
    int getX() const { return x; }
    int getY() const { return y; }
    float getDeviationSum() const { return deviationSum; }
    float getDeviationSumAverage() const { return deviationSum / deviationCounter; }
    void AddDeviation(float deviation) { deviationSum += deviation; }
    void resetDeviationSum() { deviationSum = 0; }

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
    int constainsMac(String findMac) // returns index of mac, not found = -1
    {
        for (int i = 0; i < getMeasurementCount(); i++)
        {
            if (findMac == mac.at(i))
            {
                return i;
            }
        }
        return -1;
    }
    void clearMeasurements()
    {
        rssi.clear();
        mac.clear();
    }
};
struct Location
{
    float x;
    float y;
};

String Serial_ask(String question = "") // leest tot '\n' karakter
{

    Serial.println(question);
    while (Serial.read() != -1)
        ; // leeg buffer

    while (Serial.available() == 0)
    { // wait for data available
    }
    String line = Serial.readStringUntil('\n');
    line.trim();
    Serial.println(line);

    return line;
}

std::vector<DataPoint> datapoints;
void Scan(DataPoint &dp) // no position
{
    dp.clearMeasurements();
    int aantalNetwerken = 0;
    aantalNetwerken = WiFi.scanNetworks(false, true, true, 150);
    if (aantalNetwerken <= 0)
    {
        return;
    }

    for (int i = 0; i < aantalNetwerken; i++)
    {
        dp.addMeasurement(WiFi.RSSI(i), WiFi.BSSIDstr(i));
    }
}

void Scan(std::vector<DataPoint> &vec) // has position
{
    DataPoint dp;
    dp.setPoint(
        Serial_ask("Enter X cor").toInt(),
        Serial_ask("Enter Y cor").toInt());
    Scan(dp);
    vec.push_back(dp);
}

void printData()
{
    for (int i = 0; i < datapoints.size(); i++)
    {
        Serial.println("-------------------------------------------");
        Serial.println("meetpunt" + String(i));
        Serial.println("X = " + String(datapoints[i].getX()));
        Serial.println("Y = " + String(datapoints[i].getY()));
        Serial.println(datapoints[i].getMeasurementCount());
        for (int j = 0; j < datapoints[i].getMeasurementCount(); j++)
        {
            Serial.println(String(datapoints[i].getRssi(j)) + " - " + String(datapoints[i].getMac(j)));
        }
    }
}

Location findLocation() //
{
    float x = 0;
    float y = 0;
    DataPoint findLocation;
    Scan(findLocation);

    datapoints; // vector of datapoints global

    for (int i = 0; i < datapoints.size(); i++) // overloop alle meetpunten
    {
        datapoints.at(i).resetDeviationSum();
        for (int j = 0; j < findLocation.getMeasurementCount(); j++) // overloop alle mac's in datapoint
        {                                                            // tel de som van de fouten op
            // vind mac van scan
            // vind rssi van scan
            String mac = findLocation.getMac(j);
            int rssi = findLocation.getRssi(j);

            // zit mac die voorkomt in scan ook in datapoint
            int macIndex = datapoints.at(i).constainsMac(mac);
            if (macIndex == -1)
            {
                break;
            }

            // vind rssi van mac
            int macRssi = datapoints.at(i).getRssi(macIndex);
            // update sum of deviation
            datapoints.at(i).AddDeviation(std::abs(rssi - macRssi));
            datapoints.at(i).deviationCounter++;
        }
    }

    // find datapoint whit least deviation.
    int indexLeastDeviation = 0;

    float leastDeviation = datapoints.at(0).getDeviationSumAverage();
    for (int i = 0; i < datapoints.size(); i++)
    {
        debugln("deviation average" + String(datapoints.at(i).getDeviationSumAverage()));
        if (datapoints.at(i).getDeviationSumAverage() < leastDeviation)
        {
            leastDeviation = datapoints.at(i).getDeviationSumAverage();
            indexLeastDeviation = i;
        }
    }
    debugln("index least deviation " + String(indexLeastDeviation));

    debug("i thing am at location ");
    debug(datapoints[indexLeastDeviation].getX());
    debug(" ");
    debugln(datapoints[indexLeastDeviation].getY());
    return {(float)datapoints[indexLeastDeviation].getX(), (float)datapoints[indexLeastDeviation].getY()};
}

void test_simple_fingerprinting()
{
    datapoints;

    Scan(datapoints);
    printData();
    Scan(datapoints);
    printData();
    Scan(datapoints);
    printData();

    Serial_ask("waar ben ik");
    findLocation();
    Serial_ask("waar ben ik");
    findLocation();
}