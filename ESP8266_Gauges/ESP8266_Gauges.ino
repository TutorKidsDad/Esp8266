/*********
  Rui Santos
  Complete instructions at https://RandomNerdTutorials.com/esp8266-web-server-gauges/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <Arduino_JSON.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>

// WiFi credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// Web server and event source
AsyncWebServer server(80);
AsyncEventSource events("/events");

// JSON variable for sensor data
JSONVar readings;

// Timer variables
unsigned long lastTime = 0;  
const unsigned long timerDelay = 30000;  // 30 seconds delay

// BME280 sensor object (I2C pins: SDA = GPIO 21, SCL = GPIO 22)
Adafruit_BME280 bme; 

// Initialize BME280 sensor
void initBME() {
  if (!bme.begin(0x76)) {
    Serial.println("Error: Could not find a valid BME280 sensor, check wiring!");
    while (1);  // Stop if sensor initialization fails
  }
}

// Get sensor readings and return as JSON string
String getSensorReadings() {
  readings["temperature"] = String(bme.readTemperature());
  readings["humidity"] = String(bme.readHumidity());
  return JSON.stringify(readings);
}

// Initialize LittleFS file system
void initFS() {
  if (!LittleFS.begin()) {
    Serial.println("Error: Failed to mount LittleFS");
    return;
  }
  Serial.println("LittleFS mounted successfully");
}

// Initialize WiFi connection
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println("\nConnected! IP address: " + WiFi.localIP().toString());
}

void setup() {
  // Start serial communication for debugging
  Serial.begin(115200);

  // Initialize peripherals
  initBME();
  initWiFi();
  initFS();

  // Define web server routes
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html");  // Serve HTML file
  });

  server.serveStatic("/", LittleFS, "/");  // Serve static files (e.g., CSS, JS)

  // Provide sensor readings via HTTP GET request
  server.on("/readings", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json = getSensorReadings();
    request->send(200, "application/json", json);  // Send JSON response
  });

  // Handle client connections to EventSource
  events.onConnect([](AsyncEventSourceClient *client) {
    if (client->lastId()) {
      Serial.printf("Client reconnected! Last message ID: %u\n", client->lastId());
    }
    client->send("hello!", NULL, millis(), 10000);  // Send greeting message
  });

  // Add event handler to the server
  server.addHandler(&events);

  // Start web server
  server.begin();
}

void loop() {
  // Send sensor readings every 30 seconds
  if ((millis() - lastTime) > timerDelay) {
    events.send("ping", NULL, millis());  // Ping clients
    events.send(getSensorReadings().c_str(), "new_readings", millis());  // Send sensor data
    lastTime = millis();
  }
}
