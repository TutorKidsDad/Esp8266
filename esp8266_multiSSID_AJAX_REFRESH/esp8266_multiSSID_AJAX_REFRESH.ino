/*
 * This Code is for the ESP8266 WiFi Module. It builds a website running AJAX (Asynchronous JavaScript and XML)
 * which allows this webpage to update itself without having to refresh itself which may not be resource friendly
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into pin D1 on the ESP8266 12-E - GPIO 5
#define ONE_WIRE_BUS 5

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature DS18B20(&oneWire);
char temperatureCString[7];
char temperatureFString[7];

ESP8266WebServer server(80);
String XML;

const char *ssid = "KMtronic-Office";
const char *password = "12345678";
const char *ssid2 = "HOME";
const char *password2 = "12345678";

void buildXML() {
 // <response><sensor><id>10000000000001</id><temp>26.37</temp></sensor></response>
 getTemperature();
 XML = "<?xml version='1.0'?>";
 XML += "<response>";
 XML += "<sensor><id>10000000000001</id><temp>";
 XML += temperatureCString;
 XML += "</temp></sensor>"; //incoming data from Arduino. It means it is the value of mapping function.
 XML += "</response>";
}

void handleXML() {
 digitalWrite(LED_BUILTIN, LOW);
 buildXML();
 server.send(200,"text/xml",XML);
 digitalWrite(LED_BUILTIN, HIGH);
}

void getTemperature() {
 float tempC;
 float tempF;
 do {
 DS18B20.requestTemperatures(); 
 tempC = DS18B20.getTempCByIndex(0);
 dtostrf(tempC, 2, 2, temperatureCString);
 tempF = DS18B20.getTempFByIndex(0);
 dtostrf(tempF, 3, 2, temperatureFString);
 delay(100);
 } while (tempC == 85.0 || tempC == (-127.0));
}

void setup() {

DS18B20.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement
 pinMode(LED_BUILTIN, OUTPUT);
 digitalWrite(LED_BUILTIN, HIGH);
 Serial.begin(9600);

// scan....
 int n = WiFi.scanNetworks();
 for (int i = 0; i < n; ++i) {
 if (WiFi.SSID(i)== ssid ) {
 WiFi.begin(ssid,password); //trying to connect the modem
 break;
 }
 if (WiFi.SSID(i)== ssid2) {
 WiFi.begin(ssid2,password2); //trying to connect the modem
 break;
 }
 }

while(WiFi.status() != WL_CONNECTED) {
 delay(500);
 }
 WiFi.mode(WIFI_STA);
 Serial.println("\n\n. . . BOOTING ESP8266 . . .\n");
 Serial.print("\n");
 Serial.print("CONNECTED TO ACCESS POINT : ");
 Serial.println(ssid);
 Serial.flush();
 Serial.println(WiFi.localIP());
 //it is sending the IP to Arduino's port. So I can see the IP on Serial Monitor
 Serial.flush();
 server.on("/status.xml",handleXML);
 server.begin(); 
}

void loop() {
 server.handleClient();
}
