#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

ESP8266WebServer server(80);

int mq135Pin = A0;

int moderateThreshold = 350;
int poorThreshold = 600;

void handleRoot() {

  int airQuality = analogRead(mq135Pin);
  String status;

  if (airQuality < moderateThreshold) {
    status = "GOOD 😊";
  }
  else if (airQuality >= moderateThreshold && airQuality < poorThreshold) {
    status = "MODERATE 😐";
  }
  else {
    status = "POOR 🚨";
  }

  String html = "<!DOCTYPE html><html>";
  html += "<head><meta http-equiv='refresh' content='5'/>";
  html += "<title>IoT Air Monitor</title>";
  html += "<style>body{text-align:center;font-family:Arial;}</style></head>";
  html += "<body>";
  html += "<h1>MQ-135 Air Quality Monitor</h1>";
  html += "<h2>Air Quality Value: " + String(airQuality) + "</h2>";
  html += "<h2>Status: " + status + "</h2>";
  html += "<p>Page refreshes every 5 seconds</p>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void setup() {

  Serial.begin(115200);
  delay(1000);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
}
