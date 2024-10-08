#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include "MQ135.h"
#include "DHT.h"

// WiFi and ThingSpeak credentials
const char* ssid = "testing";  
const char* pass = "password";  
String apiKey = "2XL0I1LTDPE9D1J4"; 
const char* server = "api.thingspeak.com";
unsigned long myChannelNumber = 617957;

WiFiClient client;
TinyGPSPlus gps;
SoftwareSerial ss(D1, D2);  // GPS Serial connection
DHT dht(D3, DHT22);  // DHT22 sensor on pin D3

MQ135 gasSensor = MQ135(A0);  // MQ135 sensor on A0

void setup() {
    Serial.begin(115200);
    ss.begin(9600);  // GPS baud rate
    
    // WiFi connection
    Serial.println(F("Connecting to WiFi..."));
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(F("."));
    }
    Serial.println(F("\nWiFi connected."));
    
    ThingSpeak.begin(client);  // Initialize ThingSpeak
    dht.begin();  // Initialize DHT22 sensor
}

void sendDataToThingSpeak(float airQuality, float temp, float humidity, String latitude, String longitude) {
    // Set ThingSpeak fields
    ThingSpeak.setField(1, String(airQuality));
    ThingSpeak.setField(2, latitude);
    ThingSpeak.setField(3, longitude);
    ThingSpeak.setField(4, String(temp));
    ThingSpeak.setField(5, String(humidity));

    // Write to ThingSpeak
if (ThingSpeak.writeFields(myChannelNumber, apiKey.c_str()) == 200) {
        Serial.println(F("Data sent to ThingSpeak successfully."));
    } else {
        Serial.println(F("Error sending data to ThingSpeak."));
    }
}

void readGPSData(String &latitude, String &longitude) {
    if (gps.location.isValid()) {
        latitude = String(gps.location.lat(), 6);
        longitude = String(gps.location.lng(), 6);
    } else {
        latitude = "INVALID";
        longitude = "INVALID";
        Serial.println(F("Invalid GPS data."));
    }
}

void readSensors(float &airQuality, float &temp, float &humidity) {
    airQuality = gasSensor.getPPM();  // Get air quality in PPM
    temp = dht.readTemperature();     // Get temperature
    humidity = dht.readHumidity();    // Get humidity

    // Check if DHT readings are valid
    if (isnan(temp) || isnan(humidity)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        temp = humidity = -1;
    }
}

void loop() {
    static unsigned long lastUpdateTime = 0;
    unsigned long currentTime = millis();

    // Update sensor data and send it every 20 seconds
    if (currentTime - lastUpdateTime > 20000) {
        lastUpdateTime = currentTime;

        // Sensor readings
        float airQuality, temp, humidity;
        String latitude, longitude;
        readSensors(airQuality, temp, humidity);
        readGPSData(latitude, longitude);

        // Send data to ThingSpeak
        sendDataToThingSpeak(airQuality, temp, humidity, latitude, longitude);
    }

    // GPS data processing (Non-blocking)
    while (ss.available() > 0) {
        gps.encode(ss.read());
    }

    if (millis() > 5000 && gps.charsProcessed() < 10) {
        Serial.println(F("No GPS detected: check wiring."));
    }
}
