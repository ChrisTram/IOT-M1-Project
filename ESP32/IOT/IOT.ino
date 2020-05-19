#include <NTPClient.h>


#include <WiFiUdp.h>

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include "OneWire.h"
#include "DallasTemperature.h"
#include "net_misc.h"



WiFiUDP ntpUDP;



//Offset of 2 * 3600 for UTC+2 (Europe/Paris Time Zone) with 60 seconds update interval
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 2 * 3600, 60000);


/*============= GPIO ======================*/
const int ledPin = 19; // LED Pin
const int photo_resistor_pin = A0;
OneWire oneWire(23);
DallasTemperature tempSensor(&oneWire);

WiFiClient espClient; // Wifi
PubSubClient client(espClient) ; // MQTT client

String whoami = "Derek"; // Identification de CET ESP au sein de la flotte

//StaticJsonBuffer<200> jsonBuffer;

/*===== Sleeping Modes ======*/

#define TIME_TO_SLEEP 20
#define TIME_TO_UP 1
#define US_TO_S_FACTOR 1000000LL

RTC_DATA_ATTR int bootcount = 1 ;

RTC_DATA_ATTR int time_to_sleep1 = 30;
RTC_DATA_ATTR int temp_treshold1 = 15;
RTC_DATA_ATTR int light_treshold1 = 299;
RTC_DATA_ATTR String regime_start1 = "07:00";
RTC_DATA_ATTR String regime_end1 = "19:00";

RTC_DATA_ATTR int time_to_sleep2 = 5;
RTC_DATA_ATTR int temp_treshold2 = 20;
RTC_DATA_ATTR int light_treshold2 = 2;
RTC_DATA_ATTR String regime_start2 = "19:00";
RTC_DATA_ATTR String regime_end2 = "07:00";


//esp_err_t esp_sleep_enable_ulp_wakeup()
//esp_err_t esp_sleep_pd_config(esp_sleep_pd_domain_t domain, esp_sleep_pd_option_t option) // ESP_PD_DOMAIN_RTC_PERIPH // ESP_PD_OPTION_ON

//esp_err_t esp_sleep_enable_timer_wakeup(uint64_t time_in_us)
//esp_err_t esp_sleep_disable_wakeup_source(esp_sleep_source_t source)  // ESP_SLEEP_WAKEUP_ULP light
//esp_light_sleep_start()
//esp_deep_sleep_start()

void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_TIMER :
      Serial.println("Wakeup caused by timer");
      break;
    case ESP_SLEEP_WAKEUP_ULP :
      Serial.println("Wakeup caused by ULP program");
      break;
    default:
      Serial.printf("Wakeup was not caused by sleep : %d\n", wakeup_reason);
  }
}

int timeToInt(String t){
  int hours = t.substring(0,2).toInt();
  int minutes = t.substring(3).toInt();
  return hours * 3600 + minutes * 60;
}

/*===== MQTT broker/server and TOPICS ========*/
//const char* mqtt_server = "192.168.1.100";
const char* mqtt_server = "212.115.110.52";
const int mqtt_port = 1818;
#define TOPIC_TEMPERATURE "sensors/temp"
#define TOPIC_LED "sensors/led"
#define TOPIC_LIGHT "sensors/light"
#define TOPIC_TEMP_TRESHOLD_1 "tresholds/temp1"
#define TOPIC_TEMP_TRESHOLD_2 "tresholds/temp2"
#define TOPIC_LIGHT_TRESHOLD_1 "tresholds/light1"
#define TOPIC_LIGHT_TRESHOLD_2 "tresholds/light2"
#define TOPIC_SLEEP_TIME_1 "sleep_times/regime1"
#define TOPIC_SLEEP_TIME_2 "sleep_times/regime2"
#define TOPIC_WORKING_HOURS_START_1 "working_hours/start1"
#define TOPIC_WORKING_HOURS_END_1 "working_hours/end1"
#define TOPIC_WORKING_HOURS_START_2 "working_hours/start2"
#define TOPIC_WORKING_HOURS_END_2 "working_hours/end2"
#define TOPIC_ALERT_TEMP "alerts/temp"
#define TOPIC_ALERT_LIGHT "alerts/light"


/*============== MQTT CALLBACK ===================*/

void mqtt_pubcallback(char* topic, byte* message, unsigned int length) {

  // Byte list to String ... plus facile a traiter ensuite !
  // Mais sans doute pas optimal en performance => heap ?
  String messageTemp ;
  for (int i = 0 ; i < length ; i++) {
    messageTemp += (char) message[i];
  }

  Serial.print("Message : ");
  Serial.println(messageTemp);
  Serial.print("arrived on topic : ");
  Serial.println(topic) ;

  /*//==================== Light status ==================*/

  if (String (topic) == TOPIC_LED) {
    // Par exemple : Changes the LED output state according to the message
    Serial.print("Action : Changing light to ");
    if (messageTemp == "false") {
      Serial.println("on");
      set_pin(ledPin, HIGH);

    } else if (messageTemp == "true") {
      Serial.println("off");
      set_pin(ledPin, LOW);
    }
  }


  /*//=================== Regime 1 =====================*/

  if (String (topic) == TOPIC_TEMP_TRESHOLD_1) {
    Serial.print("Action : Changing temperature treshold for regime 1 to " + messageTemp + "°C");
    temp_treshold1 = messageTemp.toInt();
  }

  if (String (topic) == TOPIC_LIGHT_TRESHOLD_1) {
    Serial.print("Action : Changing luminosity treshold for regime 1 to " + messageTemp + "°L");
    light_treshold1 = messageTemp.toInt();
  }

  if (String (topic) == TOPIC_SLEEP_TIME_1) {
    Serial.print("Action : Changing sleeping time for regime 1 to " + messageTemp + " seconds");
    time_to_sleep1 = messageTemp.toInt();
  }

  if (String (topic) == TOPIC_WORKING_HOURS_START_1) {
    Serial.print("Action : Changing working start time for regime 1 to " + messageTemp);
    regime_start1 = messageTemp;
  }

  if (String (topic) == TOPIC_WORKING_HOURS_END_1) {
    Serial.print("Action : Changing working end time for regime 1 to " + messageTemp);
    regime_end1 = messageTemp;
  }


  /*//================== Regime 2 ======================*/

  if (String (topic) == TOPIC_TEMP_TRESHOLD_2) {
    Serial.print("Action : Changing temperature treshold for regime 2 to " + messageTemp + "°C");
    temp_treshold2 = messageTemp.toInt();
  }

  if (String (topic) == TOPIC_LIGHT_TRESHOLD_2) {
    Serial.print("Action : Changing luminosity treshold for regime 2 to " + messageTemp + "°L");
    light_treshold2 = messageTemp.toInt();
  }


  if (String (topic) == TOPIC_SLEEP_TIME_2) {
    Serial.print("Action : Changing sleeping time for regime 2 to " + messageTemp + " seconds");
    time_to_sleep2 = messageTemp.toInt();
  }


  if (String (topic) == TOPIC_WORKING_HOURS_START_2) {
    Serial.print("Action : Changing working start time for regime 2 to " + messageTemp);
    regime_start2 = messageTemp;
  }

  if (String (topic) == TOPIC_WORKING_HOURS_END_2) {
    Serial.print("Action : Changing working end time for regime 2 to " + messageTemp);
    regime_end2 = messageTemp;
  }
}

// Testing tresholds for alerts

void testAlertTemp1(int temp) {
  if (temp >= temp_treshold1) {
    char data[80];
    int32_t period = 60 * 1000l;
    String payload2 = "{\"who\": \"" + whoami + "\", \"value\": " + String(temp) + "}";
    payload2.toCharArray(data, (payload2.length() + 1));
    Serial.println(data);
    client.publish(TOPIC_ALERT_TEMP, data);
    delay(period);
    client.loop();
    
  }
}

void testAlertTemp2(int temp) {
  if (temp >= temp_treshold2) {
    char data[80];
    int32_t period = 60 * 1000l;
    String payload2 = "{\"who\": \"" + whoami + "\", \"value\": " + String(temp) + "}";
    payload2.toCharArray(data, (payload2.length() + 1));
    Serial.println(data);
    client.publish(TOPIC_ALERT_TEMP, data);
    delay(period);
    client.loop();
  }
}

void testAlertLight1(int lum) {
  if (lum >= light_treshold1) {
    char data[80];
    int32_t period = 60 * 1000l;
    String payload2 = "{\"who\": \"" + whoami + "\", \"value\": " + String(lum) + "}";
    payload2.toCharArray(data, (payload2.length() + 1));
    Serial.println(data);
    client.publish(TOPIC_ALERT_LIGHT, data);
    delay(period);
    client.loop();
  }
}

void testAlertLight2(int lum) {
  if (lum >= light_treshold2) {
    char data[80];
    int32_t period = 60 * 1000l;
    String payload2 = "{\"who\": \"" + whoami + "\", \"value\": " + String(lum) + "}";
    payload2.toCharArray(data, (payload2.length() + 1));
    Serial.println(data);
    client.publish(TOPIC_ALERT_LIGHT, data);
    delay(period);
    client.loop();
  }
}

//Starting regimes

void start_regime1() {
  Serial.println("In regime 1.");
  testAlertTemp1(get_temperature());
  esp_sleep_enable_timer_wakeup(time_to_sleep1 * US_TO_S_FACTOR);
  Serial.println("Time to sleep : " + String(time_to_sleep1) + " seconds");
  Serial.println("Going to light sleep");
  esp_deep_sleep_start();
}

void start_regime2() {
  Serial.println("In regime 2.");
  testAlertTemp2(get_temperature());
  testAlertLight2(get_luminosity());
  esp_sleep_enable_timer_wakeup(time_to_sleep2 * US_TO_S_FACTOR);
  Serial.println("Time to sleep : " + String(time_to_sleep2) + " seconds");
  Serial.println("Going to deep sleep");
  esp_deep_sleep_start();
}


/*============= MQTT SUBSCRIBE =====================*/

void mqtt_mysubscribe(char* topic) {
      Serial.println("Subscribing to topic : " + String(topic));
      client.subscribe(topic); // and then Subscribe   
}

/*============= ACCESSEURS ====================*/

float get_temperature() {
  float temperature;
  tempSensor.requestTemperaturesByIndex(0);
  delay (750);
  temperature = tempSensor.getTempCByIndex(0);
  return temperature;
}

float get_luminosity() {
  return analogRead(photo_resistor_pin);
}

void set_pin(int pin, int val) {
  digitalWrite(pin, val) ;
}

int get_pin(int pin) {
  return digitalRead(pin);
}

boolean get_led_status() {
  if (get_pin(ledPin) == 0) {
    return false;
  }
  return true;
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT reconnection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      // Subscribe
        mqtt_mysubscribe((char*) (TOPIC_LED));
        mqtt_mysubscribe((char*) (TOPIC_TEMP_TRESHOLD_1));
        mqtt_mysubscribe((char*) (TOPIC_TEMP_TRESHOLD_2));
        mqtt_mysubscribe((char*) (TOPIC_LIGHT_TRESHOLD_1));
        mqtt_mysubscribe((char*) (TOPIC_LIGHT_TRESHOLD_2));
        mqtt_mysubscribe((char*) (TOPIC_SLEEP_TIME_1));
        mqtt_mysubscribe((char*) (TOPIC_SLEEP_TIME_2));
        mqtt_mysubscribe((char*) (TOPIC_WORKING_HOURS_START_1));
        mqtt_mysubscribe((char*) (TOPIC_WORKING_HOURS_START_2));
        mqtt_mysubscribe((char*) (TOPIC_WORKING_HOURS_END_1));
        mqtt_mysubscribe((char*) (TOPIC_WORKING_HOURS_END_2));
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
/*=============== SETUP =====================*/

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Boot number : " + String(bootcount));
  print_wakeup_reason();

  bootcount++;

  /* Wifi */
  connect_wifi();


  timeClient.begin();

  delay(1000);

  client.setServer(mqtt_server, mqtt_port);

  client.setCallback(mqtt_pubcallback);

}

void loop() {

  //Updating to the right time for good sync
  timeClient.update();


  /*========= Preparing payload =============*/

  char data[80];
  String payload; // Payload : "JSON ready"
  int32_t period = 60 * 1000l; // Publication period

  /* Subscribe to TOPIC_LED if not yet ! */
  if (!client.connected()) {
    reconnect();
  }

  /* Publish Temperature & Light periodically */
  payload = "{\"who\": \"";
  payload += whoami;
  payload += "\", \"value\": " ;
  payload += get_temperature();
  payload += "}";

  Serial.println("Publishing temperature mesurement.");
  payload.toCharArray(data, (payload.length() + 1)); // Convert String payload to a char array
  Serial.println(data);
  client.publish(TOPIC_TEMPERATURE, data);  // publish it

  Serial.println("Publishing light mesurement.");
  payload = "{\"who\": \"" + whoami + "\", \"value\": " + get_luminosity() + "}";
  payload.toCharArray(data, (payload.length() + 1));
  Serial.println(data);
  client.publish(TOPIC_LIGHT, data);

  Serial.println("Publishing led status.");
  payload = "{\"who\": \"" + whoami + "\", \"value\": " + get_led_status() + "}";
  payload.toCharArray(data, (payload.length() + 1));
  Serial.println(data);
  client.publish(TOPIC_LED, data);

  delay(period);

  //client.loop(); // Process MQTT ... obligatoire une fois par loop()

  int now_time = timeClient.getHours()*3600 + timeClient.getMinutes()*60;
  int start1 = timeToInt(regime_start1);
  int end1 = timeToInt(regime_end1);
  int start2 = timeToInt(regime_start2);
  int end2 = timeToInt(regime_end2);

  Serial.println("now time : " + String(now_time));
  Serial.println("start time : " + String(start2));
  Serial.println("end time : " + String(end2));

  if (start1 <= end1) {
    // start and stop times are in the same day
    if (now_time >= start1 && now_time <= end1) {
      start_regime1();
    }
  } else {
    // start and stop times are in different days
    if (now_time >= start1 || now_time <= end1) {
      start_regime1();
    }
  }

  if (start2 <= end2) {
    // start and stop times are in the same day
    if (now_time >= start2 && now_time <= end2) {
      start_regime2();
    }
  } else {
    // start and stop times are in different days
    if (now_time >= start2 || now_time <= end2) {
      start_regime2();
    }
  }
}
