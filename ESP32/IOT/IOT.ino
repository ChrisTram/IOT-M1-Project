#include <NTPClient.h>


#include <WiFiUdp.h>

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include "OneWire.h"
#include "DallasTemperature.h"
#include "net_misc.h"

/* SI NOUS AVIONS EU A DISPOSITION LE KIT GSM
#include "SIM900.h"
#include <SoftwareSerial.h>
*/

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

/* SI NOUS AVIONS EU A DISPOSITION LE KIT GSM 
SoftwareSerial sim800l(7,8);
boolean sms;
*/


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
    if (messageTemp == "on") {
      Serial.println("on");
      set_pin(ledPin, HIGH);

    } else if (messageTemp == "off") {
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
    regime_start1 = messageTemp.toInt();
  }

  if (String (topic) == TOPIC_WORKING_HOURS_END_1) {
    Serial.print("Action : Changing working end time for regime 1 to " + messageTemp);
    regime_end1 = messageTemp.toInt();
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
    regime_start2 = messageTemp.toInt();
  }

  if (String (topic) == TOPIC_WORKING_HOURS_END_2) {
    Serial.print("Action : Changing working end time for regime 2 to " + messageTemp);
    regime_end2 = messageTemp.toInt();
  }
}

// Testing tresholds for alerts

void testAlertTemp1(int temp) {
  if (temp >= temp_treshold1) {
    char data[80];
    String payload2 = "{\"who\": \"" + whoami + "\", \"value\": " + temp + "}";
    payload2.toCharArray(data, (payload2.length() + 1));
    Serial.println(data);
    client.publish(TOPIC_ALERT_TEMP, data);
  }
}

void testAlertTemp2(int temp) {
  if (temp >= temp_treshold2) {
    char data[80];
    String payload2 = "{\"who\": \"" + whoami + "\", \"value\": " + temp + "}";
    payload2.toCharArray(data, (payload2.length() + 1));
    Serial.println(data);
    client.publish(TOPIC_ALERT_TEMP, data);
  }
}

void testAlertLight1(int lum) {
  if (lum >= light_treshold1) {
    char data[80];
    String payload2 = "{\"who\": \"" + whoami + "\", \"value\": " + lum + "}";
    payload2.toCharArray(data, (payload2.length() + 1));
    Serial.println(data);
    client.publish(TOPIC_ALERT_LIGHT, data);
  }
}

void testAlertLight2(int lum) {
  if (lum >= light_treshold2) {
    char data[80];
    String payload2 = "{\"who\": \"" + whoami + "\", \"value\": " + lum + "}";
    payload2.toCharArray(data, (payload2.length() + 1));
    Serial.println(data);
    client.publish(TOPIC_ALERT_LIGHT, data);
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

  while (!client.connected()) { // Loop until we are reconnected
    Serial.print("Attempting MQTT connection...");
    if (client.connect("esp32", "try", "try")) { // Attempt to connect
      Serial.println("connected");
      client.subscribe(topic); // and then Subscribe
    } else { // Connection failed
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5 * 1000);
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

float get_luminosity() {
  return analogRead(photo_resistor_pin);
}

void set_pin(int pin, int val) {
  digitalWrite(pin, val) ;
}

int get_pin(int pin) {
  return digitalRead(pin);
}

String get_led_status() {
  if (get_pin(ledPin) == 0) {
    return "off";
  }
  return "on";
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

  client.setServer(mqtt_server, 1883);

  client.setCallback(mqtt_pubcallback);

   /* SI NOUS AVIONS EU A DISPOSITION LE KIT GSM 
  sim800l.begin(9600);  // Démarrage du modem
  Serial.begin(9600);   // Initialisation de la communication série
  delay(500);           // Délai connexion
  sms = true;
  if(sim800l.available())
    Serial.write(sim800l.read());
  client.setCallback(mqtt_pubcallback);

*/

}

 /* SI NOUS AVIONS EU A DISPOSITION LE KIT GSM 
void sendsms(){
  // SMS mode
  Serial.println("SMS sender");
  sim800l.print("AT+CMGF=1r");    // initialise le mode SMS
  delay(100);
  // numero telephone
  Serial.println("0625855265"); 
  char number[20] ;
  readSerial(number);
  sim800l.print("AT+CMGS=");
  sim800l.print(number);
  sim800l.print("r");
  delay(100);
  Serial.print("FAIRE ATTENTION A LA TEMPERATURE TROP HAUTE");
  Serial.println(number);
  char message[200];
  readSerial(message);
  sim800l.println(message);
  sim800l.print(char(26));
  delay(100);
  sim800l.println();
  Serial.print("Message : ");
  Serial.println(message);
  Serial.println("Text send");
 }
int readSerial(char result[]){
  int i = 0;
  while (1)
  {
  while (Serial.available() > 0){
    char inChar = Serial.read();
    if (inChar == 'n')
    {
  result[i] = '';
  Serial.flush();
  return 0;
  }
  if (inChar != 'r'){
    result[i] = inChar;
    i++;
*/


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
 /* SI NOUS AVIONS EU A DISPOSITION LE KIT GSM  
    if(get_temperature()>40){   // on ne passe qu’une seule fois dans le loop()
    sendsms();
  sms = false;
}  
   */
  if (regime_start1 <= regime_end1) {
    // start and stop times are in the same day
    if (now_time >= regime_start1 && now_time <= regime_end1) {
      start_regime1();
    }
  } else {
    // start and stop times are in different days
    if (now_time >= regime_start1 || now_time <= regime_end1) {
      start_regime1();
    }
  }

  if (regime_start2 <= regime_end2) {
    // start and stop times are in the same day
    if (now_time >= regime_start2 && now_time <= regime_end2) {
      start_regime2();
    }
  } else {
    // start and stop times are in different days
    if (now_time >= regime_start2 || now_time <= regime_end2) {
      start_regime2();
    }
  }
}
