//*********https://randomnerdtutorials.com/wifimanager-with-esp8266-autoconnect-custom-parameter-and-manage-your-ssid-and-password/
//modified by Swapnil Karambelkar

#include <FS.h> // Ensure this is first for SPIFFS
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h> // Use ArduinoJson 6.x
#include <DHT.h>
#include <WiFiClientSecure.h> // For SSL connection

#define DHTPIN 0 // DHT11 connected to GPIO 0
// Declare the WiFiServer instance
WiFiServer server(80);  // Port 80 for HTTP
DHT dht(DHTPIN, DHT11);
WiFiClient client;

// Variable to store the HTTP request and status
String header;
String outputState = "off";

// Assign GPIO pin for output
char output[2] = "5";
char mqtt_server[40];
char apiKey[40]; // Moved from hardcoded to JSON configuration

bool shouldSaveConfig = false;

// Callback to notify us of the need to save config
void saveConfigCallback() {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  // Initialize SPIFFS for reading config
  Serial.println("Mounting file system...");
  if (SPIFFS.begin()) {
    Serial.println("Mounted file system");
    if (SPIFFS.exists("/config.json")) {
      // Read existing configuration
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("Reading config file");
        StaticJsonDocument<512> jsonDoc;
        DeserializationError error = deserializeJson(jsonDoc, configFile);
        if (!error) {
          Serial.println("Parsed JSON successfully");
          strcpy(output, jsonDoc["output"]);
          strcpy(mqtt_server, jsonDoc["mqtt_server"]);
          strcpy(apiKey, jsonDoc["apiKey"]);
        } else {
          Serial.println("Failed to parse JSON");
        }
      }
    }
  } else {
    Serial.println("Failed to mount FS");
  }

  // Setup WiFiManager
  WiFiManagerParameter custom_output("output", "Output", output, 2);
  WiFiManagerParameter custom_mqtt_server("server", "MQTT Server", mqtt_server, 40);
  WiFiManagerParameter custom_api_key("apikey", "API Key", apiKey, 40);

  WiFiManager wifiManager;
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  // Add parameters for WiFiManager
  wifiManager.addParameter(&custom_output);
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_api_key);

  wifiManager.autoConnect("AutoConnectAP");

  Serial.println("Connected to WiFi");

  strcpy(output, custom_output.getValue());
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(apiKey, custom_api_key.getValue());

  // Save configuration if flag is set
  if (shouldSaveConfig) {
    Serial.println("Saving config");
    StaticJsonDocument<512> jsonDoc;
    jsonDoc["output"] = output;
    jsonDoc["mqtt_server"] = mqtt_server;
    jsonDoc["apiKey"] = apiKey;

    File configFile = SPIFFS.open("/config.json", "w");
    if (configFile) {
      serializeJson(jsonDoc, configFile);
      configFile.close();
    } else {
      Serial.println("Failed to open config file for writing");
    }
  }
    // Start the server
    server.begin();
  // Initialize GPIO output pin
  pinMode(atoi(output), OUTPUT);
  digitalWrite(atoi(output), LOW);
}

void loop() {
  // Non-blocking logic using millis()
  static unsigned long previousMillis = 0;
  const long interval = 500; // 500 ms for sensor readings
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Read DHT sensor values
    float humi = dht.readHumidity();
    float temp = dht.readTemperature();

    if (isnan(humi) || isnan(temp)) {
      Serial.println("Failed to read from DHT sensor!");
    } else {
      Serial.print("Temperature: ");
      Serial.print(temp);
      Serial.print(" C, Humidity: ");
      Serial.print(humi);
      Serial.println(" %");
    }

    // Send data to ThingSpeak using HTTP POST
    if (client.connect(mqtt_server, 80)) {
      String sendData = String(apiKey) + "&field1=" + String(temp) + "&field2=" + String(humi) + "\r\n\r\n";

      client.print("POST /update HTTP/1.1\n");
      client.print("Host: api.thingspeak.com\n");
      client.print("Connection: close\n");
      client.print("X-THINGSPEAKAPIKEY: " + String(apiKey) + "\n");
      client.print("Content-Type: application/x-www-form-urlencoded\n");
      client.print("Content-Length: " + String(sendData.length()) + "\n\n");
      client.print(sendData);

      Serial.println("Data sent to ThingSpeak");
    } else {
      Serial.println("Failed to connect to ThingSpeak");
    }

    client.stop(); // Close connection
  }

  // Web server functionality
WiFiClient webClient = server.available();
  if (webClient) {
    Serial.println("New Client connected");
    String currentLine = "";
    while (webClient.connected()) {
      if (webClient.available()) {
        char c = webClient.read();
        Serial.write(c);
        header += c;

        if (c == '\n') {
          if (currentLine.length() == 0) {
            // Send the response
            webClient.println("HTTP/1.1 200 OK");
            webClient.println("Content-type:text/html");
            webClient.println("Connection: close");
            webClient.println();

            // Control GPIO via web
            if (header.indexOf("GET /output/on") >= 0) {
              Serial.println("Output ON");
              outputState = "on";
              digitalWrite(atoi(output), HIGH);
            } else if (header.indexOf("GET /output/off") >= 0) {
              Serial.println("Output OFF");
              outputState = "off";
              digitalWrite(atoi(output), LOW);
            }

            // Send HTML response
            webClient.println("<!DOCTYPE html><html>");
            webClient.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            webClient.println("<style>html { font-family: Helvetica; text-align: center;}</style></head>");
            webClient.println("<body><h1>ESP8266 Web Server</h1>");
            webClient.println("<p>Output - State " + outputState + "</p>");
            if (outputState == "off") {
              webClient.println("<p><a href=\"/output/on\"><button>ON</button></a></p>");
            } else {
              webClient.println("<p><a href=\"/output/off\"><button>OFF</button></a></p>");
            }
            webClient.println("</body></html>");

            // Break the loop
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    webClient.stop();
    Serial.println("Client disconnected");
  }
}
