#include <Arduino.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <TaskScheduler.h>
#include <ESP8266Ping.h>
#include <AsyncMqttClient.h>
#include <ArduinoJson.h>
#include "utils.h"

#define  LED_WATCHDOG 2

#define STQ  15 // connect to Std pin
#define Q1   16 // connect to Q4 pin
#define Q2   14 // connect to Q3 pin
#define Q3   12 // connect to Q2 pin
#define Q4   13 // connect to Q1 pin

void wifiConnect();
void connectToMqtt();
void toggleLed();
void detectDtmfTone();

Task wifiConnectTask(1000, TASK_ONCE, &wifiConnect);
Task mqttReconnectTask(2000, TASK_ONCE, &connectToMqtt);
Task toggleLedTask(1000, TASK_FOREVER, &toggleLed);
Task dtmfToneTask(40, TASK_FOREVER, &detectDtmfTone);

WiFiManager wm;

//WiFiManagerParameter mqtt_port_param; // global param ( for non blocking w params )
//WiFiManagerParameter mqtt_address_param; // global param ( for non blocking w params )
char mqtt_server_str[40];
char mqtt_port_str[6] = "8080";
WiFiManagerParameter mqtt_address_param("server", "mqtt server", mqtt_server_str, 40);
WiFiManagerParameter mqtt_port_param("port", "mqtt port", mqtt_port_str, 6);
Scheduler runner;
std::vector<String> myQueue;

AsyncMqttClient mqttClient;
uint16 mqtt_port;

char msg[256];
uint16 signal_was;
void publish_dtmf_tone(char key);

void testPing(String host){
  if(Ping.ping(host.c_str())) {
    Serial.println("Ping to mqtt server ok!!");
  } else {
    Serial.println("Ping to mqtt server nok!!");
  }
}
/********************************/
/* MQTT Callbacks*/
void onMqttConnect(bool sessionPresent) {
  mqttReconnectTask.disable();
  runner.deleteTask(mqttReconnectTask);
  toggleLedTask.enable();
  //dtmfToneTask.enable();
  Serial.println("Connected to MQTT.");
  Serial.printf("Session present: %d\n", (int)sessionPresent);

}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {

  //toggleLedTask.enable();
  dtmfToneTask.disable();
  Serial.printf("Disconnected from MQTT. %d\n", (int)reason);
  if (WiFi.isConnected()) {
    runner.addTask(mqttReconnectTask);
    mqttReconnectTask.setIterations(TASK_ONCE);
    mqttReconnectTask.enable();
    Serial.println("mqttReconnectTask Start");
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  qos: ");
  Serial.println(properties.qos);
  Serial.print("  dup: ");
  Serial.println(properties.dup);
  Serial.print("  retain: ");
  Serial.println(properties.retain);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  index: ");
  Serial.println(index);
  Serial.print("  total: ");
  Serial.println(total);
}

void onMqttPublish(uint16_t packetId) {
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void mqttPublish(String payload) {

  if(!mqttClient.connected()){
    return;
  }
  mqttClient.publish("ksoft/event/dtmf", 0, true, payload.c_str());
}
/*****************************************************************
*  Task callback definition
*/
  void toggleLed(){
    digitalWrite(LED_WATCHDOG, !digitalRead(LED_WATCHDOG));
    String payload = "|";
    //mqttPublish(payload);
    //Serial.println("led toggled ...");
    publish_dtmf_tone('|');
    //Serial.println("led toggled ...");
  }

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  digitalWrite(LED_WATCHDOG, !digitalRead(LED_WATCHDOG));
  testPing(mqtt_address_param.getValue());

  digitalWrite(LED_WATCHDOG, !digitalRead(LED_WATCHDOG));
  mqttClient.connect();
  digitalWrite(LED_WATCHDOG, !digitalRead(LED_WATCHDOG));
}


void wifiConnect() {
    digitalWrite(LED_WATCHDOG, !digitalRead(LED_WATCHDOG));
    wifiConnectTask.disable();
    runner.deleteTask(wifiConnectTask);
    WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
    //reset settings - wipe credentials for testing
    //wm.resetSettings();
    // set dark theme
    //wm.setClass("invert");
    // add a custom input field
    int customFieldLength = 40;
    new (&mqtt_address_param) WiFiManagerParameter("mqtt_address_id", "MQTT Address", "192.168.1.200", customFieldLength,"placeholder=\"MQTT address Placeholder\"");
    wm.addParameter(&mqtt_address_param);
    new (&mqtt_port_param) WiFiManagerParameter("mqtt_port_id", "MQTT Port", "1883", customFieldLength,"placeholder=\"MQTT port Placeholder\"");
    wm.addParameter(&mqtt_port_param);
    //wm.setSaveParamsCallback(saveParamCallback);
    // auto generated AP name from chipid with password
    bool ret;
    ret = wm.autoConnect();
    if (ret){
      digitalWrite(LED_WATCHDOG, !digitalRead(LED_WATCHDOG));
      Serial.println("Wifi Connected");
      //start MQTT task
      String mqtt_address = String(mqtt_address_param.getValue());
      //Serial.println("connected1");
      IPAddress ip;
      ip.fromString(mqtt_address_param.getValue());
      mqtt_port = atoi(mqtt_port_param.getValue());
      //Serial.println("connected3");
      sprintf(msg, "Try to connect to MQTT broker address %s:%d", mqtt_address.c_str(), mqtt_port);
      Serial.println(msg);
      mqttClient.setServer(ip, mqtt_port);
      mqttReconnectTask.enable();
    }
 }

 /***********************************************/
 void publish_dtmf_tone(char key){
   StaticJsonDocument<200> doc;
   char str[18] = "";
   Utils::timeToString(str, sizeof(str));
   String payload;
   doc["timestamp"]=str;
   doc["dtmf_key"]=String(key);
   serializeJson(doc, payload);
   mqttPublish(payload);

 }

 void detectDtmfTone(){
    uint8_t number_pressed;
    bool signal;
    signal = digitalRead(STQ);
    char key;

    if(signal == HIGH && signal_was == LOW) {
      signal_was = HIGH;
      number_pressed = ( 0x00 | (digitalRead(Q1)<<0) | (digitalRead(Q2)<<1) | (digitalRead(Q3)<<2) | (digitalRead(Q4)<<3) );
      switch (number_pressed){
        case 0x01:
          Serial.println("Button Pressed =  1");
          key = '1';
        break;
        case 0x02:
          Serial.println("Button Pressed =  2");
          key = '2';
        break;
        case 0x03:
          Serial.println("Button Pressed =  3");
          key = '3';
        break;
        case 0x04:
          Serial.println("Button Pressed =  4");
          key = '4';
        break;
        case 0x05:
          Serial.println("Button Pressed =  5");
          key = '5';
        break;
        case 0x06:
          Serial.println("Button Pressed =  6");
          key = '6';
        break;
        case 7:
          Serial.println("Button Pressed =  7");
          key = '7';
        break;
        case 0x08:
          Serial.println("Button Pressed =  8");
          key = '8';
        break;
        case 0x09:
          Serial.println("Button Pressed =  9");
          key = '9';
        break;
        case 0x0A:
          Serial.println("Button Pressed =  0");
          key = '0';
        break;
        case 0x0B:
          Serial.println("Button Pressed =  *");
          key = '*';
        break;
        case 0x0C:
          Serial.println("Button Pressed =  #");
          key = '#';
        break;
        default:
          Serial.println("Unknow Button Pressed");
          key = '?';
      }
      publish_dtmf_tone(key);

    } else if (signal == LOW) {
      signal_was = LOW;
    }
  }
 /**************/


 void setup() {
       pinMode(LED_WATCHDOG, OUTPUT);

       pinMode(STQ, INPUT); // connect to Std pin
       pinMode(Q4, INPUT); // connect to Q4 pin
       pinMode(Q3, INPUT); // connect to Q3 pin
       pinMode(Q2, INPUT); // connect to Q2 pin
       pinMode(Q1, INPUT); // connect to Q1 pin
       signal_was = LOW;

       digitalWrite(LED_WATCHDOG, !digitalRead(LED_WATCHDOG));
       Serial.begin(115200);
       mqttClient.onConnect(onMqttConnect);
       mqttClient.onDisconnect(onMqttDisconnect);
       mqttClient.onSubscribe(onMqttSubscribe);
       mqttClient.onUnsubscribe(onMqttUnsubscribe);
       mqttClient.onMessage(onMqttMessage);
       mqttClient.onPublish(onMqttPublish);
      Serial.println("Initialized mqtt");

       runner.init();
       Serial.println("Initialized scheduler");
       runner.addTask(wifiConnectTask);
       wifiConnectTask.enable();
       Serial.println("added and enable wifiConnectTask");
       runner.addTask(mqttReconnectTask);
       runner.addTask(toggleLedTask);
       runner.addTask(dtmfToneTask);

       Serial.println("Ksoft tone spy started");

  }

void loop() {
      runner.execute();

}
