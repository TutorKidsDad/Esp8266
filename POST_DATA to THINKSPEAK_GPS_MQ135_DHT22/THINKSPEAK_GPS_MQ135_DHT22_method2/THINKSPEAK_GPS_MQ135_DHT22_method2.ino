#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include "MQ135.h"
#include "DHT.h"

// Configuration
const char* ssid = "testing";         // WiFi Name
const char* pass = "password";        // WiFi Password
const char* server = "api.thingspeak.com";
const String apiKey = "2XL0I1LTDPE9D1J4"; // ThingSpeak API Key
unsigned long myChannelNumber = 617957;   // ThingSpeak Channel Number

// Sensor setup
#define DHTPIN D3                    // DHT pin
DHT dht(DHTPIN, DHT22);
MQ135 gasSensor(A0);

// GPS Setup
TinyGPSPlus gps;
SoftwareSerial ss(4, 5);  // GPS RX (D2), TX (D1)
const uint32_t GPSBaud = 9600;

// WiFi client
WiFiClient client;

// Timing
unsigned long lastUpdateTime = 0;
const unsigned long postingInterval = 20000;  // 20 seconds

void setup() {
    Serial.begin(115200);
    delay(100);

    dht.begin();
    ss.begin(GPSBaud);

    // Connect to WiFi
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");

    ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {
    unsigned long currentMillis = millis();
    
    // Read GPS Data
    while (ss.available() > 0) {
        gps.encode(ss.read());
    }

    // Check if it’s time to update ThingSpeak
    if (currentMillis - lastUpdateTime >= postingInterval) {
        // Read sensor data
        float temperature = dht.readTemperature();
        float humidity = dht.readHumidity();
        int air_quality = gasSensor.getPPM();

        // Handle invalid DHT readings
        if (isnan(temperature) || isnan(humidity)) {
            Serial.println("Failed to read from DHT sensor!");
            return;
        }

        // Handle GPS data
        String latStr, lonStr;
        if (gps.location.isValid()) {
            latStr = String(gps.location.lat(), 6);
            lonStr = String(gps.location.lng(), 6);
            Serial.print("GPS - Lat: "); Serial.print(latStr);
            Serial.print(", Lon: "); Serial.println(lonStr);
        } else {
            Serial.println("GPS data not valid.");
            latStr = "0.0";
            lonStr = "0.0";
        }

        // Build POST data to send to ThingSpeak
        String postData = apiKey;
        postData += "&field1=" + String(air_quality);
        postData += "&field2=" + latStr;
        postData += "&field3=" + lonStr;
        postData += "&field4=" + String(temperature);
        postData += "&field5=" + String(humidity);

        // Send the data to ThingSpeak
        if (client.connect(server, 80)) {
            client.print("POST /update HTTP/1.1\n");
            client.print("Host: api.thingspeak.com\n");
            client.print("Connection: close\n");
            client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
            client.print("Content-Type: application/x-www-form-urlencoded\n");
            client.print("Content-Length: " + String(postData.length()) + "\n\n");
            client.print(postData);

            Serial.println("Data sent to ThingSpeak:");
            Serial.println(postData);
        } else {
            Serial.println("Failed to connect to ThingSpeak.");
        }

        // Close the connection
        client.stop();

        // Update last update time
        lastUpdateTime = currentMillis;
    }

    // Handle if no GPS is detected after 5 seconds
    if (millis() > 5000 && gps.charsProcessed() < 10) {
        Serial.println("No GPS detected: check wiring.");
    }
}
