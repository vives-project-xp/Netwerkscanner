//Dit stuurt Json data naar een database van mariadb, het werkt met een script die op de vm draait
//Dus als je deze code wil laten werken met een andere database heb je een ander script nodig en database

#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "devbit";//naam wifi netwerk
const char* password = "Dr@@dloos!";//wachtwoord wifi netwerk

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://10.20.10.19:8080/esp/scan.php"); //ip adress en poort waar het PHP script draait
    http.addHeader("Content-Type", "application/json");

    //Voorbeeld JSON data, in een het echt zou deze data dynamisch gegenereerd worden op basis van de resultaten van een WiFi scan
    String json = R"(
{
  "scan_time_start": "2026-03-12T15:00:00",
  "scan_time_end": "2026-03-12T15:00:05",
  "networks": [
    {
      "ssid": "School-WiFi",
      "bssid": "AA:BB:CC:DD:EE:01",
      "rssi": -45,
      "channel": 6,
      "security": "WPA2"
    },
    {
      "ssid": "School-WiFi",
      "bssid": "AA:BB:CC:DD:EE:02",
      "rssi": -60,
      "channel": 6,
      "security": "WPA2"
    },
    {
      "ssid": "School-WiFi",
      "bssid": "AA:BB:CC:DD:EE:03",
      "rssi": -72,
      "channel": 6,
      "security": "WPA2"
    },
    {
      "ssid": "",
      "bssid": "11:22:33:44:55:66",
      "rssi": -80,
      "channel": 1,
      "security": "WPA2"
    },
    {
      "ssid": "Guest",
      "bssid": "22:33:44:55:66:77",
      "rssi": -67,
      "channel": 11,
      "security": "WPA3"
    },
    {
      "ssid": "TeacherNet",
      "bssid": "33:44:55:66:77:88",
      "rssi": -50,
      "channel": 3,
      "security": "WPA2"
    },
    {
      "ssid": "",
      "bssid": "44:55:66:77:88:99",
      "rssi": -90,
      "channel": 9,
      "security": "WEP"
    }
  ]
}
)";


    int httpCode = http.POST(json);

    Serial.print("HTTP Response: "); 
    Serial.println(httpCode);

    http.end();
  }

  delay(10000);
}





