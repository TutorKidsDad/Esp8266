#include <ESP8266WiFi.h>

const char* ssid = "DNS Beat";  // Your WiFi network name
const char* password = "12345679";  // Your WiFi password

int R1 = D1; // Relay 1 pin
int R2 = D2; // Relay 2 pin
int R3 = D3; // Relay 3 pin
int R4 = D4; // Relay 4 pin

WiFiServer server(80); // Web server on port 80

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(R3, OUTPUT);
  pinMode(R4, OUTPUT);
  
  digitalWrite(R1, LOW);
  digitalWrite(R2, LOW);
  digitalWrite(R3, LOW);
  digitalWrite(R4, LOW);

  // Connect to WiFi network
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
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop() {
  WiFiClient client = server.available(); // Listen for incoming clients
  if (!client) {
    return;
  }

  Serial.println("New client");
  while (!client.available()) {
    delay(1);
  }

  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Handle relay control based on URL
  if (request.indexOf("/Relay1On") != -1) digitalWrite(R1, HIGH);
  if (request.indexOf("/Relay1Off") != -1) digitalWrite(R1, LOW);
  if (request.indexOf("/Relay2On") != -1) digitalWrite(R2, HIGH);
  if (request.indexOf("/Relay2Off") != -1) digitalWrite(R2, LOW);
  if (request.indexOf("/Relay3On") != -1) digitalWrite(R3, HIGH);
  if (request.indexOf("/Relay3Off") != -1) digitalWrite(R3, LOW);
  if (request.indexOf("/Relay4On") != -1) digitalWrite(R4, HIGH);
  if (request.indexOf("/Relay4Off") != -1) digitalWrite(R4, LOW);

  // Generate the HTML webpage
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head><title>Relay Control Webpage</title>");
  client.println("<style>");
  client.println("body { font-family: Arial, sans-serif; text-align: center; background-color: #f4f4f4; }");
  client.println("h1 { color: #333; }");
  client.println("h2 { color: #555; }");
  client.println("button { width: 100px; height: 40px; margin: 10px; font-size: 18px; cursor: pointer; border-radius: 5px; }");
  client.println(".on { background-color: #4CAF50; color: white; }");
  client.println(".off { background-color: #f44336; color: white; }");
  client.println("</style></head>");
  client.println("<body><h1>Relay Control Webpage</h1>");
  
  // Button for Relay 1
  client.println("<h2>Relay 1</h2>");
  client.println("<button class='on' onclick=\"location.href='/Relay1On'\">ON</button>");
  client.println("<button class='off' onclick=\"location.href='/Relay1Off'\">OFF</button>");
  
  // Button for Relay 2
  client.println("<h2>Relay 2</h2>");
  client.println("<button class='on' onclick=\"location.href='/Relay2On'\">ON</button>");
  client.println("<button class='off' onclick=\"location.href='/Relay2Off'\">OFF</button>");
  
  // Button for Relay 3
  client.println("<h2>Relay 3</h2>");
  client.println("<button class='on' onclick=\"location.href='/Relay3On'\">ON</button>");
  client.println("<button class='off' onclick=\"location.href='/Relay3Off'\">OFF</button>");
  
  // Button for Relay 4
  client.println("<h2>Relay 4</h2>");
  client.println("<button class='on' onclick=\"location.href='/Relay4On'\">ON</button>");
  client.println("<button class='off' onclick=\"location.href='/Relay4Off'\">OFF</button>");
  
  client.println("</body></html>");
  client.stop();
  delay(1);
}
