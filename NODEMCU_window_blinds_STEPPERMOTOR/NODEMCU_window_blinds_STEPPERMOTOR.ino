#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <FS.h>

#define DEV true

#define STEPPER_EN_PIN   13
#define STEPPER_DIR_PIN  2
#define STEPPER_STEP_PIN 5

#define STEPPER_RPM 600
#define STEPPER_GEAR_RATIO 64
#define STEPPER_MICROSTEP 16
#define STEPPER_FULL_ROTATION 7000

#define SERVER_URL "example.com"
#define SERVER_PORT 80
#define SERVER_CONNECTION_INTERVAL 10000

#define WIFI_DATA_FILENAME "/wifi.txt"
#define WIFI_CONNECTION_TIMEOUT 10000

ESP8266WebServer server(80);

char* ssid = new char[32];
char* password = new char[64];

bool jalousieState;

void moveStepper(unsigned long steps, bool direction) {
  unsigned long period = 60000000L / (STEPPER_RPM * STEPPER_GEAR_RATIO * STEPPER_MICROSTEP * 2UL);
  steps *= STEPPER_GEAR_RATIO;
  Serial.print("Period = ");
  Serial.println(period);
  digitalWrite(STEPPER_DIR_PIN, direction);
  digitalWrite(STEPPER_EN_PIN, LOW);
  for (int i = 0; i < steps; i++) {
    digitalWrite(STEPPER_STEP_PIN, HIGH);
    delayMicroseconds(period);
    digitalWrite(STEPPER_STEP_PIN, LOW);
    delayMicroseconds(period);
    ESP.wdtFeed();
  }
  digitalWrite(STEPPER_EN_PIN, HIGH);
}

void jalousieClose() {
  if (!jalousieState)
    return;
  Serial.println("Closing jalousie");
  moveStepper(STEPPER_FULL_ROTATION, 1);
  jalousieState = false;
}

void jalousieOpen() {
  if (jalousieState)
    return;
  Serial.println("Opening jalousie");
  moveStepper(STEPPER_FULL_ROTATION, 0);
  jalousieState = true;
}

void jalousieHome() {
  moveStepper(STEPPER_FULL_ROTATION, 1);
  jalousieState = false;
}

void handleWiFiCredentials() {
  if (!server.hasArg("SSID") || !server.hasArg("password")) { server.send(500, "text/plain", "Bag arguments"); return; }

  Serial.print("WiFi Credentials:\nSSID: ");
  Serial.println(server.arg("SSID"));
  Serial.print("Password: ");
  Serial.println(server.arg("password"));

  File wifiData = SPIFFS.open(WIFI_DATA_FILENAME, "w");
  wifiData.println(server.arg("SSID"));
  wifiData.println(server.arg("password"));
  wifiData.close();

  server.send(200, "text/plain", "Data saved");
  ESP.reset();
}

bool loadWiFiCredentials() {
  File wifiData = SPIFFS.open(WIFI_DATA_FILENAME, "r");
  if (!wifiData)
    return false;

  String _ssid = wifiData.readStringUntil('\n');
  _ssid.trim();
  _ssid.toCharArray(ssid, 32);

  String _password = wifiData.readStringUntil('\n');
  _password.trim();
  _password.toCharArray(password, 64);

  wifiData.close();
  return true;
}

void setup() {
  // Make the watch dog timeout longer
  ESP.wdtEnable(65535);

  Serial.begin(74880);
  Serial.setDebugOutput(DEV);

  pinMode(STEPPER_EN_PIN, OUTPUT);
  pinMode(STEPPER_DIR_PIN, OUTPUT);
  pinMode(STEPPER_STEP_PIN, OUTPUT);
  digitalWrite(STEPPER_EN_PIN, HIGH);

  // Initialize filesystem
  SPIFFS.begin();

  bool loaded = loadWiFiCredentials();
  if (loaded) {
    Serial.println("WiFi credentials loaded");
    unsigned long connectionTime = millis();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      if (millis() - connectionTime > WIFI_CONNECTION_TIMEOUT) {
        Serial.println("Connection failed. Ereasing old Wifi credentials and starting AP to get new credentials.");
        SPIFFS.remove(WIFI_DATA_FILENAME);
        ESP.reset();
      }
    }
  } else {
    Serial.println("WiFi credentials not found, starting AP");

    WiFi.mode(WIFI_AP);
    WiFi.softAP("Pentagon228");

    server.serveStatic("/", SPIFFS, "/public_html/WifiSetup.html");
    server.serveStatic("/WifiSetup.css", SPIFFS, "/public_html/WifiSetup.css");
    server.serveStatic("/WifiSetup.js", SPIFFS, "/public_html/WifiSetup.jsl");
    server.on("/submit", HTTP_POST, handleWiFiCredentials);

    server.begin();

    while(1) {
      server.handleClient();
      ESP.wdtFeed();
    }
  }
  jalousieHome();
}

void loop() {
  delay(SERVER_CONNECTION_INTERVAL);
  Serial.println("I'm alive");

  WiFiClient client;
  if (!client.connect(SERVER_URL, SERVER_PORT))
    return;

  client.print(String("GET / HTTP/1.1\r\n") +
                      "Host:" + SERVER_URL + ":" + SERVER_PORT + "\r\n" + 
                      "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    ESP.wdtFeed();
    delay(100);
    if (millis() - timeout > 5000) {
      client.stop();
      return;
    }
  }

  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.println("Res: " + line);
    line.trim();
    if (line.equals(String("open"))) {
      jalousieOpen();
      return;
    } else if (line.equals(String("close"))) {
      jalousieClose();
      return;
    }
  }
}
