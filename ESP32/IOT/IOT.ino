#include <NTPClient.h>
#define MYTZ TZ_Europe_Paris


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



//Offset of 2 * 3600 for UTC+2 (Europe/Paris Time Zone) with 60 seconds update interval
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 2 * 3600, 60000);


/*============= GPIO ======================*/
const int ledPin = 19; // LED Pin
const int photo_resistor_pin = A0;
OneWire oneWire(23);
DallasTemperature tempSensor(&oneWire);

WiFiClient espClient; // Wifi
PubSubClient client(espClient) ; // MQTT client

String whoami; // Identification de CET ESP au sein de la flotte

//StaticJsonBuffer<200> jsonBuffer;

/*===== Sleeping Modes ======*/

#define TIME_TO_SLEEP 20
#define TIME_TO_UP 1
#define US_TO_S_FACTOR 1000000LL

RTC_DATA_ATTR int bootcount = 1 ;

RTC_DATA_ATTR int time_to_sleep1 = 30;
RTC_DATA_ATTR int temp_treshold1 = 70;
RTC_DATA_ATTR int light_treshold1 = 1000;
RTC_DATA_ATTR int regime_start1 = 7;
RTC_DATA_ATTR int regime_end1 = 19;

RTC_DATA_ATTR int time_to_sleep2 = 40;
RTC_DATA_ATTR int temp_treshold2 = 70;
RTC_DATA_ATTR int light_treshold2 = 1000;
RTC_DATA_ATTR int regime_start2 = 19;
RTC_DATA_ATTR int regime_end2 = 7;


//esp_err_t esp_sleep_enable_ulp_wakeup()
//esp_err_t esp_sleep_pd_config(esp_sleep_pd_domain_t domain, esp_sleep_pd_option_t option) // ESP_PD_DOMAIN_RTC_PERIPH // ESP_PD_OPTION_ON

//esp_err_t esp_sleep_enable_timer_wakeup(uint64_t time_in_us)
//esp_err_t esp_sleep_disable_wakeup_source(esp_sleep_source_t source)  // ESP_SLEEP_WAKEUP_ULP light
//esp_light_sleep_start()
//esp_deep_sleep_start()

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason){
    case ESP_SLEEP_WAKEUP_TIMER :
      Serial.println("Wakeup caused by_timer");
      break;
    case ESP_SLEEP_WAKEUP_ULP :
      Serial.println("Wakeup caused by ULP program");
      break;
    default:
      Serial.printf("Wakeup was not caused by sleep : %d\n", wakeup_reason);  
  }  
}

/*===== MQTT broker/server and TOPICS ========*/
//const char* mqtt_server = "192.168.1.100";
const char* mqtt_server = "broker.hivemq.com";
#define TOPIC_TEMP "sensors/temp"
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


/*============== MQTT CALLBACK ===================*/

void mqtt_pubcallback(char* topic, byte* message, unsigned int length) {
  
  // Byte list to String ... plus facile a traiter ensuite !
  // Mais sans doute pas optimal en performance => heap ?
  String messageTemp ;
  for(int i = 0 ; i < length ; i++) {
    messageTemp += (char) message[i];
  }
  
  Serial.print("Message : ");
  Serial.println(messageTemp);
  Serial.print("arrived on topic : ");
  Serial.println(topic) ;
 
 /*//==================== Light status ==================*/
 
  if(String (topic) == TOPIC_LED) {
     // Par exemple : Changes the LED output state according to the message   
    Serial.print("Action : Changing light to ");
    if(messageTemp == "on") {
      Serial.println("on");
      set_pin(ledPin,HIGH);
     
    } else if (messageTemp == "off") {
      Serial.println("off");
      set_pin(ledPin,LOW);
    }
  }
 

  /*//=================== Regime 1 =====================*/

  if(String (topic) == TOPIC_TEMP_TRESHOLD_1){
    Serial.print("Action : Changing temperature treshold for regime 1 to " + messageTemp + "°C");
    temp_treshold1 = messageTemp.toInt();  
  }

  if(String (topic) == TOPIC_LIGHT_TRESHOLD_1){
    Serial.print("Action : Changing luminosity treshold for regime 1 to " + messageTemp + "°L");
    light_treshold1 = messageTemp.toInt();  
  }

    if(String (topic) == TOPIC_SLEEP_TIME_1){
    Serial.print("Action : Changing sleeping time for regime 1 to " + messageTemp + " seconds");
    time_to_sleep1 = messageTemp.toInt();
  }

  if(String (topic) == TOPIC_WORKING_HOURS_START_1){
    Serial.print("Action : Changing working start time for regime 1 to " + messageTemp);
    regime_start1 = messageTemp.toInt();
  }
  
  if(String (topic) == TOPIC_WORKING_HOURS_END_1){
    Serial.print("Action : Changing working end time for regime 1 to " + messageTemp);
    regime_end1 = messageTemp.toInt();
  }

  /*//================== Regime 2 ======================*/

  if(String (topic) == TOPIC_TEMP_TRESHOLD_2){
    Serial.print("Action : Changing temperature treshold for regime 2 to " + messageTemp + "°C");
    temp_treshold2 = messageTemp.toInt();
  }

  if(String (topic) == TOPIC_LIGHT_TRESHOLD_2){
    Serial.print("Action : Changing luminosity treshold for regime 2 to " + messageTemp + "°L");
    light_treshold2 = messageTemp.toInt();
  }


  if(String (topic) == TOPIC_SLEEP_TIME_2){
    Serial.print("Action : Changing sleeping time for regime 2 to " + messageTemp + " seconds");
    time_to_sleep2 = messageTemp.toInt();
  }


  if(String (topic) == TOPIC_WORKING_HOURS_START_2){
    Serial.print("Action : Changing working start time for regime 2 to " + messageTemp);
    regime_start2 = messageTemp.toInt();
  }
    
  if(String (topic) == TOPIC_WORKING_HOURS_END_2){
    Serial.print("Action : Changing working end time for regime 2 to " + messageTemp);
    regime_end2 = messageTemp.toInt();
  } 
}
/*============= MQTT SUBSCRIBE =====================*/

void mqtt_mysubscribe(char* topic) {
  
  while(!client.connected()) { // Loop until we are reconnected
    Serial.print("Attempting MQTT connection...");
    if(client.connect("esp32", "try", "try")) { // Attempt to connect 
      Serial.println("connected");
      client.subscribe(topic); // and then Subscribe
    } else { // Connection failed
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5*1000);
    }
  }
}

/*============= ACCESSEURS ====================*/

float get_temperature() {
  float temperature;
  tempSensor.requestTemperaturesByIndex(0);
  delay (750);
  temperature = tempSensor.getTempCByIndex(0);
  return temperature;
}

float get_luminosity(){
  return analogRead(photo_resistor_pin);
}

void set_pin(int pin, int val){
 digitalWrite(pin, val) ;
}

int get_pin(int pin){
  return digitalRead(pin);
}

String get_led_status(){
  if(get_pin(ledPin) == 0){
    return "off";
  }
  return "on";
}

/*=============== SETUP =====================*/

void setup(){
  Serial.begin(115200);
  while(!Serial);

  Serial.println("Boot number : " + String(bootcount));
  print_wakeup_reason();

  bootcount++;
  
  /* Wifi */
  connect_wifi();
 

  timeClient.begin();
  
  delay(1000);

  client.setServer(mqtt_server, 1883);
 
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
    mqtt_mysubscribe((char*) (TOPIC_LED));
  }
  
  /* Publish Temperature & Light periodically */
  payload = "{\"who\": \"";
  payload += whoami;   
  payload += "\", \"value\": " ;
  payload += get_temperature(); 
  payload += "}";
  
  payload.toCharArray(data, (payload.length() + 1)); // Convert String payload to a char array
  Serial.println(data);
  client.publish(TOPIC_TEMP, data);  // publish it 

  payload = "{\"who\": \"" + whoami + "\", \"value\": " + get_luminosity() + "}";
  payload.toCharArray(data, (payload.length() + 1));
  Serial.println(data);
  client.publish(TOPIC_LIGHT, data);

   payload = "{\"who\": \"" + whoami + "\", \"value\": " + get_led_status() + "}";
  payload.toCharArray(data, (payload.length() + 1));
  Serial.println(data);
  client.publish(TOPIC_LED, data);

  delay(period);
  
  client.loop(); // Process MQTT ... obligatoire une fois par loop()

  int now_time = timeClient.getHours();

  disconnect_wifi();

  if (regime_start1 <= regime_end1){
    // start and stop times are in the same day
    if (now_time >= regime_start1 && now_time <= regime_end1){
      Serial.println("In regime 1.");
      esp_sleep_enable_timer_wakeup(time_to_sleep1 * US_TO_S_FACTOR);
      Serial.println("Time to sleep : " + String(time_to_sleep1) + " seconds");
      Serial.println("Going to light sleep");
      esp_deep_sleep_start();
    }
  } else {
    // start and stop times are in different days
    if (now_time >= regime_start1 || now_time <= regime_end1){
      Serial.println("In regime 1.");
      esp_sleep_enable_timer_wakeup(time_to_sleep1 * US_TO_S_FACTOR);
      Serial.println("Time to sleep : " + String(time_to_sleep1) + " seconds");
      Serial.println("Going to deep sleep");
      esp_deep_sleep_start();
    }
  }

  if (regime_start2 <= regime_end2){
    // start and stop times are in the same day
    if (now_time >= regime_start2 && now_time <= regime_end2){
      Serial.println("In regime 2.");
      esp_sleep_enable_timer_wakeup(time_to_sleep2 * US_TO_S_FACTOR);
      Serial.println("Time to sleep : " + String(time_to_sleep2) + " seconds");
      Serial.println("Going to deep sleep");
      esp_deep_sleep_start();
    }
  } else {
    // start and stop times are in different days
    if (now_time >= regime_start2 || now_time <= regime_end2){
      Serial.println("In regime 2.");
      esp_sleep_enable_timer_wakeup(time_to_sleep2 * US_TO_S_FACTOR);
      Serial.println("Time to sleep : " + String(time_to_sleep2) + " seconds");
      Serial.println("Going to deep sleep");
      esp_deep_sleep_start();
    }
  }
}
