#include <Arduino.h>
#include <WiFi.h>
#include <vector>

#include "debug.h"

class DataPoint
{
private:
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
    float GetDeviationSum() const { return deviationSum; }
    float GetDeviationSumAverage() const { return deviationSum / deviationCounter; }
    void AddDeviation(float deviation)
    {
        deviationSum += deviation;
        deviationCounter++;
    }
    void ResetDeviation()
    {
        deviationSum = 0;
        deviationCounter = 0;
    }

    void SetPoint(int xValue, int yValue)
    {
        x = xValue;
        y = yValue;
    }

    // --- RSSI / MAC toevoegen ---
    void AddMeasurement(int rssiValue, const String &macAddress)
    {
        rssi.push_back(rssiValue);
        mac.push_back(macAddress);
    }
    // --- Aantal metingen ---
    int GetMeasurementCount() const
    {
        return rssi.size();
    }

    // --- Individuele meting ophalen ---
    int GetRssi(int index) const
    {
        return rssi.at(index);
    }

    String GetMac(int index) const
    {
        return mac.at(index);
    }
    int ConstainsMac(String findMac) // returns index of mac, not found = -1
    {
        for (int i = 0; i < GetMeasurementCount(); i++)
        {
            if (findMac == mac.at(i))
            {
                return i;
            }
        }
        return -1;
    }
    void ClearMeasurements()
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

String SerialAsk(String question = "") // leest tot '\n' karakter
{

    Serial.println(question);
    while (Serial.read() != -1)
        ; // leeg buffer

    while (Serial.available() == 0)
    yield();
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
    dp.ClearMeasurements();
    int aantalNetwerken = 0;
    aantalNetwerken = WiFi.scanNetworks(false, true, true, 150);
    if (aantalNetwerken <= 0)
    {
        return;
    }

    for (int i = 0; i < aantalNetwerken; i++)
    {
        dp.AddMeasurement(WiFi.RSSI(i), WiFi.BSSIDstr(i));
    }
}

void Scan(std::vector<DataPoint> &vec) // has position
{
    DataPoint dp;
    dp.SetPoint(
        SerialAsk("Enter X cor").toInt(),
        SerialAsk("Enter Y cor").toInt());
    Scan(dp);
    vec.push_back(dp);
}
String RssiToPoints(int rssi)
{
    String container;
    for (int i = 0; i < std::abs(rssi / 5); i++)
    {
        container += ".";
    }
    return container;
}

void PrintDataRssiBssid()
{
    for (int i = 0; i < datapoints.size(); i++)
    {
        Serial.println("-------------------------------------------");
        Serial.println("meetpunt" + String(i));
        Serial.println("X = " + String(datapoints[i].getX()));
        Serial.println("Y = " + String(datapoints[i].getY()));
        Serial.println(datapoints[i].GetMeasurementCount());
        for (int j = 0; j < datapoints[i].GetMeasurementCount(); j++)
        {
            Serial.print(String(datapoints[i].GetRssi(j)) + " - " + String(datapoints[i].GetMac(j)));
            Serial.println("    " + RssiToPoints(datapoints[i].GetRssi(j)));
        }
    }
}
void PrintData()
{
    for (int i = 0; i < datapoints.size(); i++)
    {
        Serial.println("-------------------------------------------");
        Serial.println("meetpunt" + String(i));
        Serial.println("X = " + String(datapoints[i].getX()));
        Serial.println("Y = " + String(datapoints[i].getY()));
        Serial.println(datapoints[i].GetMeasurementCount());
    }
}

Location FindLocation()
{
    float x = 0;
    float y = 0;
    DataPoint findLocation;
    Scan(findLocation);

    for (int i = 0; i < datapoints.size(); i++) // overloop alle meetpunten
    {
        datapoints.at(i).ResetDeviation();
        for (int j = 0; j < findLocation.GetMeasurementCount(); j++) // overloop alle mac's in datapoint
        {                                                            // tel de som van de fouten op
            // vind mac van scan
            // vind rssi van scan
            String mac = findLocation.GetMac(j);
            int rssi = findLocation.GetRssi(j);

            // zit mac die voorkomt in scan ook in datapoint
            int macIndex = datapoints.at(i).ConstainsMac(mac);
            if (macIndex == -1)
            {
                continue;
            }

            // vind rssi van mac
            int macRssi = datapoints.at(i).GetRssi(macIndex);
            // update sum of deviation
            datapoints.at(i).AddDeviation(std::abs(rssi - macRssi));
        }
    }

    // find datapoint whit least deviation.
    int indexLeastDeviation = 0;

    float leastDeviation = datapoints.at(0).GetDeviationSumAverage();
    for (int i = 0; i < datapoints.size(); i++)
    {
        debugln("deviation average" + String(datapoints.at(i).GetDeviationSumAverage()));
        if (datapoints.at(i).GetDeviationSumAverage() < leastDeviation)
        {
            leastDeviation = datapoints.at(i).GetDeviationSumAverage();
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

void TestSimpleFingerprinting()
{
    
    printf("hallo van simple firngrprinting\n");
    Scan(datapoints);
    PrintData();
    Scan(datapoints);
    PrintData();
    Scan(datapoints);
    PrintData();

    while (SerialAsk("waar ben ik") == "f")
    {
        FindLocation();
    }
}