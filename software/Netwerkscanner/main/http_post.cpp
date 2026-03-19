#include <WiFi.h>
#include <HTTPClient.h>

void SendJsonPost(const String &payload, const char *serverUrl)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi niet verbonden!");
        return;
    }

    WiFiClient client; // BELANGRIJK: geen Secure client!
    HTTPClient http;

    http.begin(client, serverUrl); // <-- deze lijn is de fix
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0)
    {
        printf("Code: %d\n", httpResponseCode);

        String response = http.getString();
        printf("Response: %s\n", response.c_str());
    }
    else
    {
        printf("Error: %s\n", http.errorToString(httpResponseCode).c_str());
    }

    http.end();
}