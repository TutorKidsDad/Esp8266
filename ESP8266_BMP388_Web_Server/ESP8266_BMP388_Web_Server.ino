/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-bmp388-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
*********/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP3XX.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

//Uncomment if using SPI
/*#define BMP_SCK 14
#define BMP_MISO 12
#define BMP_MOSI 13
#define BMP_CS 15*/

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BMP3XX bmp;

float temp;
float pres;
float alt;

float lastTemp = 0;
float lastPres = 0;
float lastAlt = 0;

AsyncWebServer server(80);
AsyncEventSource events("/events");

unsigned long lastTime = 0;
unsigned long timerDelay = 30000;  // send readings timer

// Function to get BMP readings
void getBMPReadings(){
  if (! bmp.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }
  temp = bmp.temperature;
  pres = bmp.pressure / 100.0;
  alt = bmp.readAltitude(SEALEVELPRESSURE_HPA);
}

// Function to process variables in HTML
String processor(const String& var) {
  getBMPReadings();
  if(var == "TEMPERATURE"){
    return String(temp);
  }
  else if(var == "PRESSURE"){
    return String(pres);
  }
  else if(var == "ALTITUDE"){
    return String(alt);
  }
  else {
    return String("Invalid Input");
  }
}

// Send updates to the web client
void sendWebUpdates() {
  if (temp != lastTemp) {
    events.send(String(temp).c_str(), "temperature", millis());
    lastTemp = temp;
  }
  if (pres != lastPres) {
    events.send(String(pres).c_str(), "pressure", millis());
    lastPres = pres;
  }
  if (alt != lastAlt) {
    events.send(String(alt).c_str(), "altitude", millis());
    lastAlt = alt;
  }
}

// HTML Page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>BMP388 Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p {  font-size: 1.2rem;}
    body {  margin: 0;}
    .topnav { overflow: hidden; background-color: #0F7173; color: white; font-size: 1.4rem; }
    .content { padding: 20px; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 700px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); }
    .reading { font-size: 2rem; }
    .card.temperature { color: #272932; }
    .card.altitude { color: #D8A47F; }
    .card.pressure { color: #F05D5E; }
    .loading { animation: blinker 1s linear infinite; }
    @keyframes blinker { 50% { opacity: 0.5; } }
  </style>
</head>
<body>
  <div class="topnav">
    <h3>BMP388 WEB SERVER</h3>
  </div>
  <div class="content">
    <div class="cards">
      <div class="card pressure">
        <h4><i class="fas fa-angle-double-down"></i> PRESSURE</h4><p><span class="reading"><span id="pres" class="loading">Loading...</span> hPa</span></p>
      </div>
      <div class="card altitude">
        <h4><i class="fas fa-long-arrow-alt-up"></i> ALTITUDE</h4><p><span class="reading"><span id="alt" class="loading">Loading...</span> m</span></p>
      </div>
      <div class="card temperature">
        <h4><i class="fas fa-thermometer-half"></i> TEMPERATURE</h4><p><span class="reading"><span id="temp" class="loading">Loading...</span> &deg;C</span></p>
      </div>
    </div>
  </div>
<script>
if (!!window.EventSource) {
 var source = new EventSource('/events');
 
 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);
 
 source.addEventListener('temperature', function(e) {
  document.getElementById("temp").innerHTML = e.data;
 }, false);
 
 source.addEventListener('pressure', function(e) {
  document.getElementById("pres").innerHTML = e.data;
 }, false);
 
 source.addEventListener('altitude', function(e) {
  document.getElementById("alt").innerHTML = e.data;
 }, false);
}
</script>
</body>
</html>)rawliteral";

// Setup WiFi
void setupWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  unsigned long wifiTimeout = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    if (millis() - wifiTimeout > 10000) {
      Serial.println("WiFi connection failed! Restarting...");
      ESP.restart();
    }
  }
  Serial.print("Connected! IP Address: ");
  Serial.println(WiFi.localIP());
}

// Setup BMP sensor
void setupBMP() {
  if (!bmp.begin_I2C()) {
    Serial.println("Could not find BMP3 sensor, check wiring!");
    delay(2000);  // Allow user to read error
    ESP.restart();
  }
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
}

// Setup the server and events
void setupServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID: %u\n", client->lastId());
    }
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  server.begin();
}

void setup() {
  Serial.begin(115200);
  setupWiFi();
  setupBMP();
  getBMPReadings();  // Initial reading
  setupServer();
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    getBMPReadings();
    sendWebUpdates();
    lastTime = millis();
  }
}
