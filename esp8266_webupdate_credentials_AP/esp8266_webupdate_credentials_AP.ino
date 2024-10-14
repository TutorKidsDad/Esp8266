#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <DHT.h>  // Including library for DHT

// Constants and Settings
#define DHTPIN 0        // Pin where the DHT11 is connected
#define DHTTYPE DHT11   // DHT 11 sensor type

DHT dht(DHTPIN, DHTTYPE);  // Create DHT sensor instance

// ThingSpeak settings
const char* MQTT_SERVER = "api.thingspeak.com"; // API for ThingSpeak
const int updateInterval = 15000; // Time between updates in ms

// Default credentials (if EEPROM is empty)
const char* defaultSSID = "Default_SSID";
const char* defaultPassword = "Default_Password";
const char* defaultChannelAPIKey = "53UKHXLUSDGMLD0L";

// Variables
String ssid, password, channelAPIKey;
ESP8266WebServer server(80);
WiFiClient client;

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  // Start EEPROM and read stored credentials
  EEPROM.begin(512);
  readCredentialsFromEEPROM();

  // Connect to WiFi
  WiFi.begin(ssid.c_str(), password.c_str());
  if (connectToWiFi()) {
    Serial.println("Connected to WiFi!");
  } else {
    Serial.println("Failed to connect. Setting up AP...");
    setupAccessPoint();
    setupWebServer();
  }

  // Wait for a successful WiFi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    server.handleClient(); // Handle web requests
  }
}

void loop() {
  // Handle any web requests
  server.handleClient();

  // Read sensor data
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Send data to ThingSpeak
  if (client.connect(MQTT_SERVER, 80)) {
    String postStr = channelAPIKey + "&field1=" + String(temperature) + "&field2=" + String(humidity) + "\r\n";
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: " + String(MQTT_SERVER) + "\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + channelAPIKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: " + String(postStr.length()) + "\n\n");
    client.print(postStr);
    client.flush();
    client.stop();
    Serial.println("Data sent to ThingSpeak.");
  }

  // Wait before sending the next data packet
  delay(updateInterval);
}

// WiFi connection function
bool connectToWiFi() {
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500);
    Serial.print(".");
    retries++;
  }
  return (WiFi.status() == WL_CONNECTED);
}

// Access Point and Web Server Setup
void setupAccessPoint() {
  WiFi.softAP("ESP8266_Config");
  Serial.println("Access Point created. Connect to it to set credentials.");
}

void setupWebServer() {
  server.on("/", []() {
    String html = "<html><body><h1>ESP8266 WiFi Config</h1>"
                  "<form action='/save' method='POST'>"
                  "SSID: <input type='text' name='ssid'><br>"
                  "Password: <input type='text' name='password'><br>"
                  "ThingSpeak API Key: <input type='text' name='apikey'><br>"
                  "<input type='submit' value='Save'></form></body></html>";
    server.send(200, "text/html", html);
  });

  server.on("/save", []() {
    ssid = server.arg("ssid");
    password = server.arg("password");
    channelAPIKey = server.arg("apikey");

    // Save credentials to EEPROM
    saveCredentialsToEEPROM();

    String response = "Credentials saved. Rebooting...";
    server.send(200, "text/html", response);
    delay(2000);
    ESP.restart();  // Reboot to apply new settings
  });

  server.begin();
  Serial.println("Web server started.");
}

// EEPROM Functions
void readCredentialsFromEEPROM() {
  ssid = readStringFromEEPROM(0, 32);
  password = readStringFromEEPROM(32, 64);
  channelAPIKey = readStringFromEEPROM(64, 96);
  
  if (ssid.length() == 0) ssid = defaultSSID;
  if (password.length() == 0) password = defaultPassword;
  if (channelAPIKey.length() == 0) channelAPIKey = defaultChannelAPIKey;
}

void saveCredentialsToEEPROM() {
  writeStringToEEPROM(0, ssid);
  writeStringToEEPROM(32, password);
  writeStringToEEPROM(64, channelAPIKey);
  EEPROM.commit();
}

String readStringFromEEPROM(int start, int end) {
  String value = "";
  for (int i = start; i < end; ++i) {
    value += char(EEPROM.read(i));
  }
  return value;
}

void writeStringToEEPROM(int start, String value) {
  for (int i = start; i < (start + value.length()); ++i) {
    EEPROM.write(i, value[i - start]);
  }
  for (int i = start + value.length(); i < start + 32; ++i) {
    EEPROM.write(i, 0);
  }
}
