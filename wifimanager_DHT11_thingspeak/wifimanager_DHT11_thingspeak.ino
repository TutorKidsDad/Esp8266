#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <WiFiManager.h>  // Replaced AutoConnect with WiFiManager
#include <DHT.h>  // Including library for DHT

#define MQTT_USER_KEY        "****************"  // Replace to User API Key.
#define CHANNEL_ID           "1299395"            // Replace to Channel ID.
#define CHANNEL_API_KEY_WR   "53UKHXLUSDGMLD0L"  // Replace to the write API Key.

#define MQTT_UPDATE_INTERVAL 15000
#define MQTT_TOPIC           "channels/" CHANNEL_ID "/publish/" CHANNEL_API_KEY_WR
#define MQTT_USER_ID         "anyone" 
#define MQTT_SERVER          "mqtt.thingspeak.com"

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

#define DHTPIN D3         // Pin where the DHT11 is connected
DHT dht(DHTPIN, DHT11);

unsigned long lastPub = 0;

bool mqttConnect() {
  static const char alphanum[] = "0123456789"
                                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                 "abcdefghijklmnopqrstuvwxyz";  // For random generation of client ID.
  char clientId[9];

  uint8_t retry = 10;
  while (!mqttClient.connected()) {
    Serial.println("Attempting MQTT broker:" MQTT_SERVER);

    for (uint8_t i = 0; i < 8; i++) {
      clientId[i] = alphanum[random(62)];
    }
    clientId[8] = '\0';

    if (mqttClient.connect(clientId, MQTT_USER_ID, MQTT_USER_KEY)) {
      Serial.println("Established:" + String(clientId));
      return true;
    } else {
      Serial.print("Connection failed:" + String(mqttClient.state()));
      if (!--retry) {
        return false;  // Ensure the function returns false when retries run out
      }
    }
    delay(3000);
  }
  return false;  // Ensure the function returns false if not connected
}

void mqttPublish(String msg) {
  String path = String(MQTT_TOPIC);
  int    tLen = path.length();
  char   topic[tLen + 1];
  path.toCharArray(topic, tLen + 1);

  int    mLen = msg.length();
  char   payload[mLen + 1];
  msg.toCharArray(payload, mLen + 1);

  mqttClient.publish(topic, payload);
}

void setup() {
  // Start serial communication
  Serial.begin(115200);
  delay(1000);
  Serial.println();

  // Initialize DHT sensor
  dht.begin();

  // Initialize WiFiManager to handle Wi-Fi credentials
  WiFiManager wifiManager;

  // Auto connect and enter AP mode if credentials are not found
  wifiManager.autoConnect("ESP8266_MQTT_AP");

  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Set MQTT server details
  mqttClient.setServer(MQTT_SERVER, 1883);
}

void loop() {
  // Read humidity and temperature from DHT sensor
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(100);
    return;
  }

  // Publish to MQTT at regular intervals
  if (millis() - lastPub > MQTT_UPDATE_INTERVAL) {
    if (!mqttClient.connected()) {
      mqttConnect();
    }

    String postStr = CHANNEL_API_KEY_WR;
    postStr += "&field1=";
    postStr += String(t);
    postStr += "&field2=";
    postStr += String(h);
    postStr += "\r\n\r\n";
    mqttPublish(postStr);

    mqttClient.loop();
    lastPub = millis();
  }
}
