
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

String apiKey = "2XL0I1LTDPE9D1J4"; // Enter your Write API key from ThingSpeak
const char *ssid = "testing"; // replace with your wifi ssid and wpa2 key
const char *pass = "password";
const char* server = "api.thingspeak.com";



const int buttonPin = D0;     // SMOKE IN 
int buttonState = 0;         // variable for reading the pushbutton status
WiFiClient client;
SoftwareSerial ss(4, 5);
void setup()
{
pinMode(buttonPin, INPUT);
pinMode(0, OUTPUT);
pinMode(2, OUTPUT);
pinMode(D5, OUTPUT);
Serial.begin(9600);
ss.begin(9600);
delay(10);
Serial.println("Connecting to ");
Serial.println(ssid);
WiFi.begin(ssid, pass);
if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi lost connection. Reconnecting...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
}

Serial.println("");
Serial.println("WiFi connected");
Serial.println("AT");
delay(1000);
ss.println("AT");
delay(1000);

}
void loop()
{
buttonState = digitalRead(buttonPin);
float h = analogRead(A0);
if (isnan(h))
{
Serial.println("Failed to read from MQ-5 sensor!");
return;
}
//////////////////////////////////////////Sensor 1////////////////////////////////////////////////////////

if (client.connect(server, 80)) // "184.106.153.149" or api.thingspeak.com
{
digitalWrite(0, HIGH); 
delay(200);
String postStr = apiKey;
postStr += "&field1=";
postStr += String(h/1023*100);
postStr += "r\n";
client.print("POST /update HTTP/1.1\n");
client.print("Host: api.thingspeak.com\n");
client.print("Connection: close\n");
client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
client.print("Content-Type: application/x-www-form-urlencoded\n");
client.print("Content-Length: ");
client.print(postStr.length());
client.print("\n\n");
client.print(postStr);
Serial.print("Gas Level 1: ");
Serial.println(h/1023*100);
Serial.println("Data Send to Thingspeak");
delay(1000);
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Sensor 2////////////////////////////////////////////////////////

if (client.connect(server, 80)) // "184.106.153.149" or api.thingspeak.com
{
digitalWrite(0, LOW); 
delay(200);
String postStr = apiKey;
postStr += "&field2=";
postStr += String(h/1023*100);
postStr += "r\n";
client.print("POST /update HTTP/1.1\n");
client.print("Host: api.thingspeak.com\n");
client.print("Connection: close\n");
client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
client.print("Content-Type: application/x-www-form-urlencoded\n");
client.print("Content-Length: ");
client.print(postStr.length());
client.print("\n\n");
client.print(postStr);
Serial.print("Gas Level 2: ");
Serial.println(h/1023*100);
Serial.println("Data Send to Thingspeak");
delay(1000);
}
/////////////////////////////////////////////////Sensor 0/////////////////////////////////////////////////

if (client.connect(server, 80)) // "184.106.153.149" or api.thingspeak.com
{
digitalWrite(D5, HIGH); 
delay(200);
String postStr = apiKey;
postStr += "&field4=";
postStr += String(h/1023*100);
postStr += "r\n";
client.print("POST /update HTTP/1.1\n");
client.print("Host: api.thingspeak.com\n");
client.print("Connection: close\n");
client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
client.print("Content-Type: application/x-www-form-urlencoded\n");
client.print("Content-Length: ");
client.print(postStr.length());
client.print("\n\n");
client.print(postStr);
Serial.print("Watering of field ON: ");
Serial.println(h/1023*100);
Serial.println("Data Send to Thingspeak");
delay(1000);
}
//////////////////////////////////////////Sensor 3////////////////////////////////////////////////////////

if (client.connect(server, 80)) // "184.106.153.149" or api.thingspeak.com
{

String postStr = apiKey;
postStr += "&field3=";
postStr += String(buttonState);
postStr += "r\n";
client.print("POST /update HTTP/1.1\n");
client.print("Host: api.thingspeak.com\n");
client.print("Connection: close\n");
client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
client.print("Content-Type: application/x-www-form-urlencoded\n");
client.print("Content-Length: ");
client.print(postStr.length());
client.print("\n\n");
client.print(postStr);
Serial.print("Smoke 2: ");
Serial.println("Smoke Data Send to Thingspeak");
delay(1000);
}
//////////////////////////////////////////////////////////////////////////////////////////////////
if (buttonState == HIGH)
{
   digitalWrite(2, HIGH);
   delay(5000);

 //////1//////
  ss.println("AT");
  delay(2000);
  ss.print("ATD9826015410;\r");
  delay(20000);
  ss.print("ATH \r"); 
  delay(1000);
  //////////////
   //////2//////
 
  ss.println("AT");
  delay(2000);
  ss.print("ATD7771960785;\r");
  delay(20000);
  ss.print("ATH \r"); 
  delay(1000);
  
  //////////////
   //1///
  ss.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(2000);  // Delay of 1000 milli seconds or 1 second
  ss.println("AT+CSCS=\"GSM\"");
  //Serial.print(gsm);
  delay(1000);
  ss.println("AT+CMGS=\"+916266391521\"\r"); // Replace x with mobile number
  delay(2000);
  ss.println("Alert! Please Check for fire in the field");// The SMS text you want to send
  delay(100);
  ss.println((char)26);// 
  delay(1000);
    //2///
            // Check GSM response
        while (ss.available()) {
            String response = ss.readString();
            Serial.println(response);
        }
    digitalWrite(2, LOW);
}
///////////////////////////////////////////
//////////////////////////////////////////weight calculate////////////////////////////////////////////////////////

if (client.connect(server, 80)) // "184.106.153.149" or api.thingspeak.com
{
digitalWrite(0, HIGH); 
delay(200);
String postStr = apiKey;
postStr += "&field5=";
postStr += String((1.3464)/(h/1023*100));
postStr += "r\n";
client.print("POST /update HTTP/1.1\n");
client.print("Host: api.thingspeak.com\n");
client.print("Connection: close\n");
client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
client.print("Content-Type: application/x-www-form-urlencoded\n");
client.print("Content-Length: ");
client.print(postStr.length());
client.print("\n\n");
client.print(postStr);
Serial.print("Quantity required: ");
Serial.println((1.3464)/(h/1023*100));
Serial.println("Data Send to Thingspeak");
delay(1000);
}
//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
client.stop();
// thingspeak needs minimum 15 sec delay between updates.
delay(15000);
}
