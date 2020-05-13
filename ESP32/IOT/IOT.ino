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

/*===== MQTT broker/server and TOPICS ========*/
//const char* mqtt_server = "192.168.1.100";
const char* mqtt_server = "broker.hivemq.com";
#define TOPIC_TEMP "sensors/temp"
#define TOPIC_LED "sensors/led"
#define TOPIC_LIGHT "sensors/light"

/*===== Sleeping Modes ======*/

#define TIME_TO_SLEEP 20
#define TIME_TO_UP 1
#define US_TO_S_FACTOR 1000000LL

RTC_DATA_ATTR int bootcount = 1 ;
RTC_DATA_ATTR int time_to_sleep1 = 60;

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

void set_local_time(){
  int rcode = setenv("TZ","Europe/Paris",1); // "CET-1CEST,M3.5.0,M10.5.0/3"
  tzset();
  Serial.println(rcode);
}

void print_local_time(){  
  char buffer[30];
  struct timeval tv;
  time_t curtime;
  gettimeofday(&tv, NULL); 
  curtime=tv.tv_sec;
  strftime(buffer,30,"%m-%d-%Y  %T.",localtime(&curtime));
  Serial.printf("%s%ld\n",buffer,tv.tv_usec);
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
 

  esp_err_t esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * US_TO_S_FACTOR);
  Serial.println("Time to sleep : " + String(TIME_TO_SLEEP) + " seconds");

  //esp_err_t esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_SLEEP_WAKEUP_ULP);

  Serial.println("Going to light sleep");
  //esp_light_sleep_start();
  
}

void loop() {
  timeClient.update();
  //set_local_time();
  //print_local_time();
  Serial.println(timeClient.getFormattedTime());

  delay(1000);
}
