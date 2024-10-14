/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-mqtt-publish-ds18b20-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>

// Replace these with your Wi-Fi credentials
#define WIFI_SSID "REPLACE_WITH_YOUR_SSID"
#define WIFI_PASSWORD "REPLACE_WITH_YOUR_PASSWORD"

// MQTT broker details (Replace IP address or use domain for cloud-based brokers)
#define MQTT_HOST IPAddress(192, 168, 1, 000) // Change as needed
#define MQTT_PORT 1883

// MQTT topic for temperature data
#define MQTT_PUB_TEMP "esp/ds18b20/temperature"

// GPIO where the DS18B20 is connected
const int oneWireBus = 4;          
OneWire oneWire(oneWireBus);          // OneWire instance to communicate with DS18B20
DallasTemperature sensors(&oneWire);  // DallasTemperature instance for DS18B20

float temp;  // Store temperature value

AsyncMqttClient mqttClient;  // MQTT Client instance
Ticker mqttReconnectTimer;   // Timer to reconnect MQTT
Ticker wifiReconnectTimer;   // Timer to reconnect Wi-Fi

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;

unsigned long previousMillis = 0;   // Track last time temperature was published
const long interval = 10000;        // Interval to publish temperature (10 seconds)

// Function to connect to Wi-Fi
void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

// Called when Wi-Fi is connected
void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.println("Connected to Wi-Fi.");
  connectToMqtt();
}

// Called when Wi-Fi is disconnected
void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // Ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  wifiReconnectTimer.once(2, connectToWifi); // Retry Wi-Fi connection after 2 seconds
}

// Function to connect to MQTT broker
void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

// Called when MQTT is connected
void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
}

// Called when MQTT is disconnected
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");
  if (WiFi.isConnected()) {
    mqttReconnectTimer.once(2, connectToMqtt); // Retry MQTT connection after 2 seconds
  }
}
/*void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
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
}*/
// Called when an MQTT message is successfully published
void onMqttPublish(uint16_t packetId) {
  Serial.print("Publish acknowledged. Packet ID: ");
  Serial.println(packetId);
}

void setup() {
  Serial.begin(115200);
  sensors.begin(); // Initialize the temperature sensor
  
  // Handle Wi-Fi connection events
  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);
  
  // Set MQTT callbacks
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onPublish(onMqttPublish);
  
  mqttClient.setServer(MQTT_HOST, MQTT_PORT); // Set MQTT server and port
  
  // Connect to Wi-Fi
  connectToWifi();
}

void loop() {
  unsigned long currentMillis = millis();

  // Publish new temperature reading every interval (10 seconds)
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    sensors.requestTemperatures(); // Request temperature from sensor
    temp = sensors.getTempCByIndex(0); // Get temperature in Celsius

    // Check if the temperature reading is valid
    if (temp != DEVICE_DISCONNECTED_C) {
      // Publish temperature to MQTT topic
      uint16_t packetIdPub1 = mqttClient.publish(MQTT_PUB_TEMP, 1, true, String(temp).c_str());
      Serial.printf("Publishing on topic %s at QoS 1, packetId: %i. ", MQTT_PUB_TEMP, packetIdPub1);
      Serial.printf("Message: %.2f°C\n", temp);
    } else {
      Serial.println("Error: Could not read temperature from sensor.");
    }
  }
}
