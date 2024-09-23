#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN D5          // DHT11 Data pin connected to GPIO14 (D5)
#define DHTTYPE DHT11      // DHT 11
#define TRIGPIN D6         // Ultrasonic Trig pin connected to GPIO12 (D6)
#define ECHOPIN D7         // Ultrasonic Echo pin connected to GPIO13 (D7)

// Create DHT sensor instance
DHT dht(DHTPIN, DHTTYPE);

// I2C LCD (16x2) initialization
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Set the LCD address to 0x27 for a 16 chars and 2 line display

// Wi-Fi credentials
const char* ssid = "Your_SSID";
const char* password = "Your_PASSWORD";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  
  // Initialize LCD and print a message
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Connecting to");
  lcd.setCursor(0, 1);
  lcd.print("WiFi...");

  // Initialize DHT11 sensor
  dht.begin();
  
  // Initialize ultrasonic sensor pins
  pinMode(TRIGPIN, OUTPUT);
  pinMode(ECHOPIN, INPUT);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    
    // Keep displaying "Connecting to WiFi..." until connected
    lcd.setCursor(0, 1);
    lcd.print("WiFi...");
  }
  
  // Display IP address once connected
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connected! IP:");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());

  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // Start the web server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float distance = getUltrasonicDistance();

    // Send response to web client
    String htmlResponse = "<html><body>";
    htmlResponse += "<h1>ESP8266 Sensor Data</h1>";
    htmlResponse += "<p><b>Temperature:</b> " + String(t) + " °C</p>";
    htmlResponse += "<p><b>Humidity:</b> " + String(h) + " %</p>";
    htmlResponse += "<p><b>Distance:</b> " + String(distance) + " cm</p>";
    htmlResponse += "</body></html>";
    
    request->send(200, "text/html", htmlResponse);

    // Update LCD with sensor values
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: " + String(t) + "C");
    lcd.setCursor(0, 1);
    lcd.print("Hum: " + String(h) + "%");
  });

  // Start the server
  server.begin();
}

float getUltrasonicDistance() {
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGPIN, LOW);
  
  long duration = pulseIn(ECHOPIN, HIGH);
  float distance = duration * 0.034 / 2;
  return distance;
}

void loop() {
  // Nothing required in the loop as AsyncWebServer runs in the background
}
