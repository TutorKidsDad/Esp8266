/*********
  Rui Santos
  Complete instructions at https://RandomNerdTutorials.com/esp8266-nodemcu-plot-readings-charts-multiple/

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <Arduino_JSON.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Replace with your network credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create an Event Source on /events
AsyncEventSource events("/events");

// Json Variable to hold sensor readings
JSONVar readings;

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 10000;  // 10 seconds delay for updating sensor readings

// GPIO where the DS18B20 sensors are connected to
const int oneWireBus = 4;

// Setup a oneWire instance to communicate with OneWire devices (DS18B20)
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

// Address of each sensor
DeviceAddress sensor1 = { 0x28, 0xFF, 0xB4, 0x6, 0x33, 0x17, 0x3, 0x4B };
DeviceAddress sensor2 = { 0x28, 0xFF, 0x43, 0xF5, 0x32, 0x18, 0x2, 0xA8 };
DeviceAddress sensor3 = { 0x28, 0xFF, 0xA0, 0x11, 0x33, 0x17, 0x3, 0x96 };
DeviceAddress sensor4 = { 0x28, 0xFF, 0x11, 0x28, 0x33, 0x18, 0x1, 0x6B };

// Get Sensor Readings and return JSON object
String getSensorReadings() {
  sensors.requestTemperatures();

  JSONVar readings;  // Create a new JSONVar object

  readings["sensor1"] = (sensors.getTempC(sensor1) == DEVICE_DISCONNECTED_C) ? "Error" : String(sensors.getTempC(sensor1));
  readings["sensor2"] = (sensors.getTempC(sensor2) == DEVICE_DISCONNECTED_C) ? "Error" : String(sensors.getTempC(sensor2));
  readings["sensor3"] = (sensors.getTempC(sensor3) == DEVICE_DISCONNECTED_C) ? "Error" : String(sensors.getTempC(sensor3));
  readings["sensor4"] = (sensors.getTempC(sensor4) == DEVICE_DISCONNECTED_C) ? "Error" : String(sensors.getTempC(sensor4));

  String jsonString = JSON.stringify(readings);
  return jsonString;
}


// Initialize LittleFS
void initLittleFS() {
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
  } else {
    Serial.println("LittleFS mounted successfully");
  }
}

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());
}

// Check WiFi connection
void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Reconnecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print('.');
      WiFi.reconnect();
    }
    Serial.println("Reconnected to WiFi!");
    Serial.println(WiFi.localIP());
  }
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  initWiFi();
  initLittleFS();

  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html");
  });

  server.serveStatic("/", LittleFS, "/");

  // Request for the latest sensor readings
  server.on("/readings", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "application/json", getSensorReadings());
  });

  // Event Source for client connections
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);

  // Start server
  server.begin();
}

void loop() {
  checkWiFiConnection();  // Ensure WiFi is connected

  unsigned long currentMillis = millis();
  if (currentMillis - lastTime > timerDelay) {
    lastTime = currentMillis;

    // Send ping and sensor readings
    events.send("ping", NULL, currentMillis);
    events.send(getSensorReadings().c_str(), "new_readings", currentMillis);
  }
}
