/* 
Programm tp send the data from the sensor DHT11 to a php page and from there to google forms
*/

#include <DHT.h> 
#include <ESP8266WiFi.h>

#define CYCLE 1000//10 minutes, change as desired
#define DHTPIN 2
#define DHTTYPE DHT11 // DHT 11
const int buttonPin = 3;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin
const char* ssid = ""; // "ur SSID";
const char* password = ""; //"ur wifi pswd";
const char* host = "www.onlinetps.com";
unsigned long value = 0;

unsigned int lastcall = CYCLE;
int conn_time;
// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status

DHT dht(DHTPIN, DHTTYPE);

void setup() {
Serial.begin(115200);
delay(10);
// initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
dht.begin();

// We start by connecting to a WiFi network

Serial.println();
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);

WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
conn_time++;
if (conn_time > 20) {
break;
}
}
if (WiFi.status() == WL_CONNECTED) {
Serial.println("");
Serial.println("WiFi connected");
Serial.println("IP address: ");
Serial.println(WiFi.localIP());
}
else {
Serial.println("");
Serial.println("no WiFi connection");
ESP.deepSleep(100000);
Serial.println("gone to sleep");
}
}

void loop() {
while (millis() - lastcall < CYCLE) {
delay(1);
}
lastcall = millis();
++value;

Serial.print("connecting to ");
Serial.println(host);

// Use WiFiClient class to create TCP connections
WiFiClient client;
const int httpPort = 80;
if (!client.connect(host, httpPort)) {
Serial.println("connection failed");
return;
}
{
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
    digitalWrite(ledPin, HIGH);
  } else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
  }
}



float h = dht.readHumidity();
// Read temperature as Celsius (the default)
float t = dht.readTemperature();

Serial.println(h);
Serial.println(t);

// We now create a URI for the request
String url = "http://onlinetps.com/store/XXX.php?";
url += "h=";
url += h;
url += "&t=";
url += t;
url += "&h1=";
url += buttonState;
url += "&t1=";
url += buttonState;
//url += value;

Serial.print("Requesting URL: ");
Serial.println(url);


/* // This will send the request to the server
client.print(String("GET ") + url + " HTTP/1.1rn" +
"Host: " + host + "rn" +
"Connection: closernrn");*/

client.print(String("GET ") + url + " HTTP/1.1\r\n" +
           "Host: " + host + "\r\n" + 
           "Connection: close\r\n\r\n");
delay(25000);

// Read all the lines of the reply from server and print them to Serial
while (client.available()) {
String line = client.readStringUntil('r');
Serial.print(line);
}

Serial.println();
Serial.println("closing connection");
}
