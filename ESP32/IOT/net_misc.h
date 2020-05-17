#include <WiFi.h>

void print_ip_status()
{
    Serial.print("\nWiFi connected !\n");
    Serial.print("IP address: ");
    Serial.print(WiFi.localIP());
    Serial.print("\n");
    Serial.print("MAC address: ");
    Serial.print(WiFi.macAddress());
    Serial.print("\n");
}

void connect_wifi()
{
    // Access Point of the infrastructure
    const char *ssid = "WIFI-JRKY";
    const char *password = "kaoutyasreda06";
    const char *whoami = "Reda";

    Serial.println("\nConnecting Wifi to ");
    Serial.println(ssid);

    Serial.print("Attempting to connect ");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }
    print_ip_status();
}

void disconnect_wifi()
{
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}
