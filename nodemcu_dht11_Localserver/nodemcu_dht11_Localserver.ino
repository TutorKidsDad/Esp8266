//https://iotmonk.com/lessons/read-temperature-humidity-nodemcu-esp12e-dht11-show-html-webpagelocal-part1/

#include <ESP8266WiFi.h>
#include "DHT.h"

#define DHTPIN D3
#define DHTTYPE DHT22 

const char* ssid = "testing";
const char* password = "password";

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(10);
  dht.begin();
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}
void loop() {
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  
  // Read the first line of the request
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();


  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  if (req.indexOf("/data") != -1){
    client.flush();
    client.println("HTTP/1.1 200 OK");           // This tells the browser that the request to provide data was accepted
    client.println("Access-Control-Allow-Origin: *");  //Tells the browser it has accepted its request for data from a different domain (origin).
    client.println("Content-Type: application/json;charset=utf-8");  //Lets the browser know that the data will be in a JSON format
    client.println("Server: Arduino");           // The data is coming from an Arduino Web Server (this line can be omitted)
    client.println("Connection: close");         // Will close the connection at the end of data transmission.
    client.println();                            // You need to include this blank line - it tells the browser that it has reached the end of the Server reponse header.
                               // This is tha starting bracket of the JSON data
    client.print("{\"temperature\": \"");
    client.print(t);                           
    client.print("\", \"Humidity\": \"");
    client.print(h);               
    client.print("\"}");                      
                      
  }
  else {
    client.flush();
    client.println("HTTP/1.1 200 OK");           // This tells the browser that the request to provide data was accepted
    client.println("Access-Control-Allow-Origin: *");  //Tells the browser it has accepted its request for data from a different domain (origin).
    client.println("Content-Type: application/json;charset=utf-8");  //Lets the browser know that the data will be in a JSON format
    client.println("Server: Arduino");           // The data is coming from an Arduino Web Server (this line can be omitted)
    client.println("Connection: close");         // Will close the connection at the end of data transmission.
    client.println();                            // You need to include this blank line - it tells the browser that it has reached the end of the Server reponse header.
                          // This is tha starting bracket of the JSON data
    client.print("{\"Response\": ");
    client.print("Invalid");                          
    client.print("}");                     
                       // This is the final bracket of the JSON data
  }

    delay(1);
    Serial.println("Client disonnected");

}
