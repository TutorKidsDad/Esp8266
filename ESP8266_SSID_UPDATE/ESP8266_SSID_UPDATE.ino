#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager

// Set web server port number to 80
WiFiServer server(80);

// HTTP Request header variable
String header;

// Output states
const String STATE_ON = "on";
const String STATE_OFF = "off";
String output5State = STATE_OFF;
String output4State = STATE_OFF;

// GPIO Pins
const int output5 = 5;
const int output4 = 4;

// Function to initialize GPIO pins
void initGPIO() {
  pinMode(output5, OUTPUT);
  pinMode(output4, OUTPUT);
  digitalWrite(output5, LOW);  // Initialize pin to LOW
  digitalWrite(output4, LOW);  // Initialize pin to LOW
}

// Function to handle GPIO on/off requests
void handleGPIO(String path, int pin, String& state) {
  if (path.indexOf("/" + String(pin) + "/on") >= 0) {
    Serial.println("GPIO " + String(pin) + " on");
    state = STATE_ON;
    digitalWrite(pin, HIGH);
  } else if (path.indexOf("/" + String(pin) + "/off") >= 0) {
    Serial.println("GPIO " + String(pin) + " off");
    state = STATE_OFF;
    digitalWrite(pin, LOW);
  }
}

void setup() {
  Serial.begin(115200);
  
  // Initialize GPIO
  initGPIO();

  // WiFiManager for handling connection
  WiFiManager wifiManager;
  
  // Uncomment to reset saved Wi-Fi credentials
  // wifiManager.resetSettings();
  
  // Setup Wi-Fi AutoConnect
  wifiManager.autoConnect("AutoConnectAP");
  Serial.println("Connected to WiFi");

  // Start web server
  server.begin();
}

void loop() {
  WiFiClient client = server.available(); // Listen for incoming clients
  
  if (client) {  
    Serial.println("New Client");
    String currentLine = "";
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;

        if (c == '\n') {
          // End of client request
          if (currentLine.length() == 0) {
            // Send the HTTP response headers
            client.println(F("HTTP/1.1 200 OK"));
            client.println(F("Content-type:text/html"));
            client.println(F("Connection: close"));
            client.println();

            // Handle GPIO 5 and 4 operations
            handleGPIO(header, 5, output5State);
            handleGPIO(header, 4, output4State);

            // Build and send the web page
            client.println(F("<!DOCTYPE html><html><head>"));
            client.println(F("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"));
            client.println(F("<link rel=\"icon\" href=\"data:,\">"));
            client.println(F("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center; }"));
            client.println(F(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;"));
            client.println(F("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer; }"));
            client.println(F(".button2 { background-color: #77878A; }"));
            client.println(F("</style></head><body><h1>ESP8266 Web Server</h1>"));

            // Display buttons and state for GPIO 5
            client.println(F("<p>GPIO 5 - State ") + output5State + F("</p>"));
            if (output5State == STATE_OFF) {
              client.println(F("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>"));
            } else {
              client.println(F("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>"));
            }

            // Display buttons and state for GPIO 4
            client.println(F("<p>GPIO 4 - State ") + output4State + F("</p>"));
            if (output4State == STATE_OFF) {
              client.println(F("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>"));
            } else {
              client.println(F("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>"));
            }
            client.println(F("</body></html>"));

            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";  // Clear the header variable
    client.stop();  // Close the connection
    Serial.println("Client disconnected");
  }
}
