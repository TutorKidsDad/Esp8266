#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Ultrasonic pin definitions
const int ULTRASONIC_TRIG_PIN = 5;  // TRIG to D1
const int ULTRASONIC_ECHO_PIN = 4;  // ECHO to D2

int outputValue = 0;

// User config
const char* wifi_ssid = "smartdustbin";             // WiFi SSID
const char* wifi_password = "12345678";             // WiFi Password
const char* apiKeyIn = "ZqJAHN2gY9pDFzfs3TXU6ORXFasVFLMg";  // API Key

// ASKSENSORS config
String host = "http://asksensors.com";

// Create a WiFiClient object for HTTPClient
WiFiClient wifiClient;

// Function to reconnect WiFi
void reconnectWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Reconnecting to WiFi...");
    WiFi.begin(wifi_ssid, wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nReconnected.");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("*****************************************************");
  Serial.println("Program Start : Ultrasonic Smart Dustbin with Logging");

  // Start WiFi connection
  WiFi.begin(wifi_ssid, wifi_password);
  Serial.print("Connecting to WiFi: ");
  Serial.println(wifi_ssid);
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\n-> WiFi connected");
  Serial.print("-> IP address: ");
  Serial.println(WiFi.localIP());

  // Ultrasonic sensor setup
  pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
}

void sendDataToServer(int distanceValue) {
  // Ensure WiFi is connected before proceeding
  reconnectWiFi();
  
  // Create an HTTP client instance
  HTTPClient http;

  // Construct the URL
  String url = host + "/api.asksensors/write/" + apiKeyIn + "?module1=" + String(distanceValue);
  
  Serial.print("Sending request to URL: ");
  Serial.println(url);

  // Begin HTTP request with WiFiClient
  http.begin(wifiClient, url);
  
  // Send the GET request
  int httpCode = http.GET();

  // Check if request was successful
  if (httpCode > 0) {
    Serial.printf("[HTTP] Response code: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println("Server response: " + payload);
    }
  } else {
    Serial.printf("[HTTP] GET failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  // End the HTTP connection
  http.end();
}

void loop() {
  // Measure distance using ultrasonic sensor
  long duration, distance;
  
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  
  duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH);
  distance = (duration / 2) / 29.1;  // Distance in cm
  
  // Map the distance to percentage
  outputValue = map(distance, 2, 20, 100, 0);  // Ensure correct range mapping
  outputValue = constrain(outputValue, 0, 100);  // Constrain output to valid percentage range
  
  Serial.print("Distance to get full: ");
  Serial.print(distance);
  Serial.println(" cm");
  Serial.print("Mapped value: ");
  Serial.println(outputValue);
  
  // Send data to the server
  sendDataToServer(outputValue);
  
  // Wait 20 seconds before the next reading
  delay(20000);
}
