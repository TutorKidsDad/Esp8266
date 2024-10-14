#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

String readString;
const char* ssid = "OnlineTPS";
const char* password = "TPS@12345";
const int buttonPin = 2;          // the number of the pushbutton pin
int buttonState;                  // the current reading from the input pin
int lastButtonState = LOW;        // the previous reading from the input pin
unsigned long lastDebounceTime = 0;   // the last time the output pin was toggled
unsigned long debounceDelay = 50;     // the debounce time; increase if the output flickers

const char* host = "script.google.com";
const int httpsPort = 443;
WiFiClientSecure client;  // Use WiFiClientSecure class for TLS connection

String GAS_ID = "AKfycbyw_TSyh_tPjvUloug9NVmPsny9eQUH3gf9zntWaHyMXKLbDHTV";   // Replace by your GAS service id 

void setup() {
  pinMode(buttonPin, INPUT);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  sendData(200, 201, 202, 203, 204, 205, 206, 2007);  // Send test data
}

void loop() {
  int reading = digitalRead(buttonPin);

  // Debounce logic for button press
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      // Send data when button is pressed
      if (buttonState == HIGH) {
        sendData(100, 101, 102, 103, 104, 105, 106, 1007);
      }
    }
  }

  lastButtonState = reading;

  while (Serial.available()) {
    char c = Serial.read();  // Get one byte from the serial buffer
    readString += c;  // Build the string
    delay(2);  // Slow down loop to allow buffer to fill with the next character
  }

  if (readString.length() > 0) {
    Serial.println(readString);  // Print the captured string
    int n = readString.toInt();
    int n1 = readString.toInt();
    int n2 = readString.toInt();
    int n3 = readString.toInt();
    int n4 = readString.toInt();
    int n5 = readString.toInt();
    int n6 = readString.toInt();

    // Send data
    sendData(analogRead(A0), n, n1, n2, n3, n4, n5, n6);
    readString = "";  // Empty string for next input
  }
}

// Function for sending data to Google Spreadsheet
void sendData(int tem, int hum, int tem1, int hum1, int tem2, int hum2, int tem3, int hum3) {
  Serial.print("Connecting to ");
  Serial.println(host);

  client.setInsecure();  // Skip certificate verification (use for trusted connections)

  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection failed");
    return;
  }

  String string_temperature = String(tem, DEC);
  String string_humidity = String(hum, DEC);
  String string_temperature1 = String(tem1, DEC);
  String string_humidity1 = String(hum1, DEC);
  String string_temperature2 = String(tem2, DEC);
  String string_humidity2 = String(hum2, DEC);
  String string_temperature3 = String(tem3, DEC);
  String string_humidity3 = String(hum3, DEC);

  String url = "/macros/s/" + GAS_ID + "/exec?temperature=" + string_temperature + "&humidity=" + string_humidity +
               "&temperature1=" + string_temperature1 + "&humidity1=" + string_humidity1 +
               "&temperature2=" + string_temperature2 + "&humidity2=" + string_humidity2 +
               "&temperature3=" + string_temperature3 + "&humidity3=" + string_humidity3;

  Serial.print("Requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: ESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("Request sent");

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("Headers received");
      break;
    }
  }

  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("ESP8266/Arduino: Data sent successfully!");
  } else {
    Serial.println("ESP8266/Arduino: Data send failed");
  }

  Serial.println("Reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("Closing connection");
}
