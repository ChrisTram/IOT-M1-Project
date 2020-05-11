#include <NTPClient.h>

//#include <WiFi.h> // for WiFi shield
//#include <WiFi101.h> // for WiFi 101 shield or MKR1000
#include <WiFiUdp.h>

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include "OneWire.h"
#include "DallasTemperature.h"
#include "net_misc.h"


WiFiUDP ntpUDP;

// By default 'pool.ntp.org' is used with 60 seconds update interval and no offset
NTPClient timeClient(ntpUDP);

// You can specify the time server pool and the offset, (in seconds)
// additionaly you can specify the update interval (in milliseconds).
// NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

/*============= GPIO ======================*/
const int ledPin = 19; // LED Pin
const int photo_resistor_pin = A0;
OneWire oneWire(23);
DallasTemperature tempSensor(&oneWire);

WiFiClient espClient; // Wifi
PubSubClient client(espClient) ; // MQTT client

String whoami; // Identification de CET ESP au sein de la flotte

//StaticJsonBuffer<200> jsonBuffer;

/*===== MQTT broker/server and TOPICS ========*/
//const char* mqtt_server = "192.168.1.100";
const char* mqtt_server = "broker.hivemq.com";
#define TOPIC_TEMP "sensors/temp"
#define TOPIC_LED "sensors/led"
#define TOPIC_LIGHT "sensors/light"

/*=============== SETUP =====================*/

void setup(){
  Serial.begin(115200);
  
  /* Wifi */
  connect_wifi();
  timeClient.begin();
}

void loop() {
  timeClient.update();

  Serial.println(timeClient.getFormattedTime());

  delay(1000);
}
