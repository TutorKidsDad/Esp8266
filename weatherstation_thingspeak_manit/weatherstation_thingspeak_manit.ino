#include <ESP8266WiFi.h>          // Include the ESP8266 WiFi library
#include <ESP8266WiFiMulti.h>     // Include library for managing multiple WiFi connections
#include "ThingSpeak.h"           // Include ThingSpeak library for data handling
#include <Wire.h>                 // Include Wire library for I2C communication
#include <Adafruit_Sensor.h>      // Include Adafruit Sensor library for sensor compatibility
#include <Adafruit_BMP085_U.h>    // Include Adafruit BMP085 Unified library for BMP085 sensor
#include <DHT.h>                  // Include DHT library for temperature and humidity sensor
#include <ArduinoOTA.h>           // Include Arduino OTA library for over-the-air updates

ESP8266WiFiMulti wifiMulti;                                  // Create a WiFiMulti object for connecting to multiple networks

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085); // Create a BMP085 sensor object

#define DHTPIN 2               // Define the pin for the DHT sensor
DHT dht(DHTPIN, DHT11, 2);     // Create a DHT object for DHT11 sensor on specified pin

unsigned long myChannelNumber = 754087;                    // ThingSpeak channel number
const char * myWriteAPIKey = "P40BOPZKO3XND6JY";          // ThingSpeak write API key

const char* server = "api.thingspeak.com";                // ThingSpeak server URL

WiFiClient client;                                        // Create a WiFiClient object for HTTP requests

void setup() {
  wifiMulti.addAP("weatherstation", "12345678");        // Add WiFi network credentials

  Serial.begin(115200);                                  // Start serial communication at 115200 baud rate

  Serial.println("Connecting ...");                       // Print connecting message
  int i = 0;                                              // Initialize counter
  while (wifiMulti.run() != WL_CONNECTED) {              // Wait for the Wi-Fi to connect
    delay(5000);                                         // Delay for 5 seconds before retrying
    Serial.print('.');                                   // Print a dot to indicate connection attempt
  }
  Serial.println('\n');                                  // Print a newline after connection
  Serial.print("SSID:\t\t");                             // Print the connected SSID
  Serial.println(WiFi.SSID());                           // Display the SSID
  Serial.print("IP address:\t");                          // Print the local IP address
  Serial.println(WiFi.localIP());                        // Display the local IP address
  dht.begin();                                          // Initialize the DHT sensor
  ThingSpeak.begin(client);                              // Initialize ThingSpeak with the WiFi client
  ArduinoOTA.begin();                                    // Initialize OTA updates
}

void loop() {
  ArduinoOTA.handle();                                   // Handle OTA requests

  delay(10000);                                          // Wait for 10 seconds between readings
  float dht_h = dht.readHumidity();                      // Read humidity from DHT sensor
  float dht_t = dht.readTemperature();                   // Read temperature from DHT sensor
  double gamma = log(dht_h / 100) + ((17.62 * dht_t) / (243.5 + dht_t)); // Calculate gamma for dew point
  double dp = 243.5 * gamma / (17.62 - gamma);         // Calculate dew point
  float dew_point = dp;                                  // Store dew point value

  if (isnan(dht_h) || isnan(dht_t)) {                   // Check for invalid readings
    Serial.println("Failed to read from DHT11");        // Print error message
    delay(5000);                                         // Wait for 5 seconds before retrying
    return;                                              // Exit the loop if readings are invalid
  } else {
    // Print temperature, humidity, and dew point values
    Serial.print("Temperature:\t");
    Serial.print(dht_t);
    Serial.print(" ºC \n");
    Serial.print("Humidity:\t");
    Serial.print(dht_h);
    Serial.print(" RH \n");
    Serial.print("Dew Point:\t");
    Serial.print(dp);
    Serial.print(" ºC \n");
  }

  int ldrValue = analogRead(A0);                        // Read analog value from light sensor
  // float voltage = ldrValue * (3.3 / 1023.0);          // Optionally convert to voltage
  float lux = ((250.0 * ldrValue) - 50) / 10000;       // Calculate light intensity in lumens
  Serial.print("Light Intensity:\t\t");                 // Print light intensity message
  Serial.print(lux);                                     // Print the calculated light intensity
  Serial.println(" lumen \n");                           // End light intensity line

  if (!bmp.begin()) {                                   // Initialize BMP sensor
    Serial.print("Failed to connect to BMP sensor");    // Print error message if connection fails
    while (1);                                          // Halt the program
  }

  sensors_event_t event;                                // Create an event object for sensor data
  bmp.getEvent(&event);                                 // Get sensor readings

  // Print pressure and altitude values
  Serial.print("Pressure:\t");
  Serial.print(event.pressure);                          // Print pressure in hPa
  Serial.print(" hPa\n");

  float seaLevelPressure = 1015;                        // Define sea level pressure for altitude calculation
  Serial.print("Altitude:\t");
  Serial.print(bmp.pressureToAltitude(seaLevelPressure, event.pressure)); // Calculate and print altitude
  Serial.println(" m");

  float temperature;                                     // Variable to hold temperature reading
  bmp.getTemperature(&temperature);                      // Get temperature from BMP sensor
  Serial.print("Temperature:\t");
  Serial.print(temperature);                             // Print temperature from BMP sensor
  Serial.print(" ºC \n");

  if (client.connect(server, 80)) {                     // Connect to ThingSpeak server
    // Set field values for ThingSpeak
    ThingSpeak.setField(1, event.pressure);             // Set field 1 to pressure
    ThingSpeak.setField(2, temperature);                // Set field 2 to BMP sensor temperature
    ThingSpeak.setField(3, lux);                        // Set field 3 to light intensity
    ThingSpeak.setField(4, dht_h);                      // Set field 4 to DHT humidity
    ThingSpeak.setField(5, bmp.pressureToAltitude(seaLevelPressure, event.pressure)); // Set field 5 to calculated altitude
    ThingSpeak.setField(6, dew_point);                  // Set field 6 to dew point

    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey); // Send data to ThingSpeak
    delay(15000);                                        // Wait for 15 seconds before next data upload
  }
}
