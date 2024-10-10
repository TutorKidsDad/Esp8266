This code is an Arduino sketch for an ESP8266 microcontroller that reads temperature and humidity data from an SHT31 sensor via I2C and publishes this data to ThingSpeak. The code also serves a web interface for real-time monitoring. It employs the TaskScheduler library to manage different tasks like I2C reading, Wi-Fi connection, and hosting a web server. Below is a detailed explanation of the code.

## Code Overview

### Libraries and Definitions

- **Wire.h**: This library is used for I2C communication.
- **TaskScheduler.h**: This library allows scheduling tasks at specific intervals, making it easier to manage multiple tasks.
- **ESP8266WiFi.h**: This library is used to manage Wi-Fi connections.
- **ESP8266WebServer.h**: This library allows the ESP8266 to act as a web server.

### Constants and Variables

```cpp
#define Addr 0x44  // SHT31 I2C address
IPAddress ap_local_IP(192,168,1,4);  // Static IP for AP mode
IPAddress ap_gateway(192,168,1,254);  // Gateway IP
IPAddress ap_subnet(255,255,255,0);   // Subnet mask
```

- The SHT31 sensor is connected via I2C at the address `0x44`.
- The ESP8266 is set up to create a static Access Point (AP) with a specific IP address, gateway, and subnet.

```cpp
const char WiFissid[] = "Accesspoint";  // SSID for the local Wi-Fi
const char WiFipass[] = "password";      // Password for the local Wi-Fi
const char APssid[] = "Accesspoint1";    // SSID for the AP
const char APpass[] = "password";         // Password for the AP
```

- These variables store the SSID and password for both the local Wi-Fi network and the access point created by the ESP8266.

```cpp
const char* hostId="Your_Things_speak_hostid"; // ThingSpeak host ID
String apiKey = "Things_speak_api_key";         // ThingSpeak API key
```

- These variables store the host ID and API key required for publishing data to ThingSpeak.

```cpp
volatile float humid; 
volatile float tempC;
volatile float tempF;
```

- These variables hold the temperature and humidity values read from the SHT31 sensor.

### Function Prototypes

The function prototypes are declared for task callbacks, which will be defined later in the code:

```cpp
void taskI2CCallback();
void taskI2CDisable();
void taskAPCallback();
void taskAPDisable();
void taskWiFiCallback();
void taskWiFiDisable();
```

### HTML Web Pages

The code contains HTML for two web pages:

1. **Home Page (`PAGE1`)**: Provides an introduction to the project and a link to the source code.
2. **Data Display Page (`PAGE2`)**: Displays the real-time temperature and humidity readings.

### Server and Scheduler Initialization

```cpp
ESP8266WebServer server(80); // Initialize the web server on port 80
Scheduler ts; // Create a Scheduler instance

// Create tasks for I2C reading, hosting web server, and posting data to ThingSpeak
Task tI2C(1 * TASK_SECOND, TASK_FOREVER, &taskI2CCallback, &ts, false, NULL, &taskI2CDisable);
Task tAP(5 * TASK_SECOND, TASK_FOREVER, &taskAPCallback, &ts, false, NULL, &taskAPDisable);
Task tWiFi(5 * TASK_SECOND, TASK_FOREVER, &taskWiFiCallback, &ts, false, NULL, &taskWiFiDisable);
```

- The web server listens on port 80.
- Three tasks are defined: 
  - `tI2C`: Reads data from the sensor every second.
  - `tAP`: Handles the access point every 5 seconds.
  - `tWiFi`: Posts data to ThingSpeak every 5 seconds.

### `setup()` Function

The `setup()` function initializes the serial communication, sets up the I2C connection, configures the web server routes, and enables the I2C task.

```cpp
void setup() {
 Serial.begin(115200);
 Serial.setTimeout(1000);
 
 for(uint8_t t=4; t>0; t--){
   Serial.printf("[SETUP] WAIT...%d \n",t);
   Serial.flush();
   delay(1000);
 }
 
 Wire.begin(2,14); // Initialize I2C with SDA on D2 and SCL on D1
 server.on("/", onHandleDataRoot); // Set up route for root page
 server.on("/Value", onHandleDataFeed); // Set up route for data page
 server.onNotFound(onHandleNotFound); // Handle 404 errors

 // Set timeout for tasks
 tI2C.setTimeout(10 * TASK_SECOND);
 tAP.setTimeout(50 * TASK_SECOND);
 tWiFi.setTimeout(50 * TASK_SECOND); 
 
 tI2C.enable(); // Enable the I2C task
}
```

### `loop()` Function

The `loop()` function continuously executes the task scheduler, allowing it to manage the different scheduled tasks.

```cpp
void loop() {
  ts.execute(); 
}
```

### Task Callbacks

#### `taskI2CCallback()`

This function reads temperature and humidity data from the SHT31 sensor via I2C. It performs the following:

1. Sends a command to the SHT31 sensor to read data.
2. Receives and processes the data, converting raw readings to Celsius and Fahrenheit for temperature and percentage for humidity.
3. Prints the readings to the serial monitor.

```cpp
void taskI2CCallback() {
  Serial.println("taskI2CStarted");
  unsigned int root[6];

  // Begin I2C transmission
  Wire.beginTransmission(Addr);
  Wire.write(0x2C); // Command for one-shot measurement
  Wire.write(0x06); // Command for high repeatability
  Wire.endTransmission();

  // Request 6 bytes from the sensor
  Wire.beginTransmission(Addr);
  Wire.endTransmission();
  Wire.requestFrom(Addr, 6);

  if (Wire.available() == 6) {
    root[0] = Wire.read();
    root[1] = Wire.read();
    root[2] = Wire.read(); // CRC not used here
    root[3] = Wire.read();
    root[4] = Wire.read();
    root[5] = Wire.read(); // CRC not used here
  }

  // Calculate temperature and humidity
  int temp = (root[0] * 256) + root[1];
  float cTemp = -45.0 + (175.0 * temp / 65535.0);
  float fTemp = (cTemp * 1.8) + 32.0;
  float humidity = (100.0 * ((root[3] * 256.0) + root[4])) / 65535.0;

  // Store values
  tempC = cTemp;
  tempF = fTemp;
  humid = humidity;

  // Print values
  Serial.print("Temperature in C:\t");
  Serial.println(String(cTemp, 1));
  Serial.print("Temperature in F:\t");
  Serial.println(String(fTemp, 1));
  Serial.print("Humidity:\t ");
  Serial.println(String(humidity, 1));
}
```

#### `taskI2CDisable()`

This function disables the I2C task and initiates the AP connection if it times out.

```cpp
void taskI2CDisable() {
  if (tI2C.timedOut()) {
    Serial.println("//taskI2C disabled");
    Serial.println("call taskAP");
    reconnectAPWiFi(); // Connect to the AP
    tAP.setCallback(&taskAPCallback);
    tAP.enable(); // Enable AP task
    tI2C.disable(); // Disable I2C task
  }   
}
```

#### `taskAPCallback()`

This function handles incoming client requests when in AP mode.

```cpp
void taskAPCallback() {
  Serial.println("taskAP started");
  server.handleClient(); // Handle web server requests
}
```

#### `taskAPDisable()`

This function disables the AP task and initiates a connection to the local Wi-Fi if it times out.

```cpp
void taskAPDisable() {
  if (tAP.timedOut()) {
    Serial.println("//taskI2C disabled");
    Serial.println("call taskAP");
    reconnectWiFi(); // Connect to local Wi-Fi
    tWiFi.setCallback(&taskWiFiCallback);
    tWiFi.enable(); // Enable Wi-Fi task
    tAP.disable(); // Disable AP task
  }
}
```

#### `taskWiFiCallback()`

This function connects to ThingSpeak and posts temperature and humidity data.

```cpp
void taskWiFiCallback() {
  WiFiClient wifiClient;
  if (wifiClient.connect(hostId, 80)) {
    String postStr = apiKey;
    postStr += "&field1="; // Humidity
    postStr += String(humid);
    postStr += "&field2="; // Temperature in Celsius
    postStr += String(tempC);
    postStr += "&field3="; // Temperature in Fahrenheit
    postStr += String(tempF);
    postStr += "\r\n\r\n";

    // Prepare HTTP POST request
    wifiClient.print("POST /update HTTP/1.1\n");
    wifiClient.print

("Host: api.thingspeak.com\n");
    wifiClient.print("Connection: close\n");
    wifiClient.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    wifiClient.print("Content-Type: application/x-www-form-urlencoded\n");
    wifiClient.print("Content-Length: ");
    wifiClient.print(postStr.length());
    wifiClient.print("\n\n");
    wifiClient.print(postStr);
  }
  wifiClient.stop(); // Close the connection
}
```

#### `taskWiFiDisable()`

This function disables the Wi-Fi task and re-enables the I2C task if it times out.

```cpp
void taskWiFiDisable() {
  if (tWiFi.timedOut()) {
    Serial.println("//taskAP disabled");
    Serial.println("call taskI2C");
    tI2C.setCallback(&taskI2CCallback);
    tI2C.enable(); // Enable I2C task again
    tWiFi.disable(); // Disable Wi-Fi task
  }
}
```

### Web Server Handlers

#### `onHandleDataRoot()`

This function serves the home page when the root URL is accessed.

```cpp
void onHandleDataRoot() {
  server.send(200, "text/html", PAGE1);
}
```

#### `onHandleDataFeed()`

This function serves the data page that displays current temperature and humidity values.

```cpp
void onHandleDataFeed() {
  String PAGE2 = 
  "<!DOCTYPE html>"
  "<html>"
  "<head>"
  "<meta charset=\"utf-8\">"
  "<meta name = \"viewport\" content = \"width = device-width, initial-scale = 1.0\">"
  "<title> My first HTML page</title>"
  "</head>"
  "<body style= \"background-color:#EDEEF2\">"
  "<h1 style= \"color:blue\">Temperature and Humidity</h1>"
  "<h3 style= \"font-family:verdana; color:blue\">Temperature in Celsius:</h3>"
  "<p style=\"font-size:160%; font-family:Lucida Console\">" + String(tempC, 1) + "</p>"
  "<h3 style= \"font-family:verdana; color:blue\">Temperature in Fahrenheit:</h3>"
  "<p style=\"font-size:160%; font-family:Lucida Console\">" + String(tempF, 1) + "</p>"
  "<h3 style= \"font-family:verdana; color:blue\">Humidity:</h3>"
  "<p style=\"font-size:160%; font-family:Lucida Console\">" + String(humid, 1) + "</p>"
  "</body>"
  "</html>";

  server.send(200, "text/html", PAGE2);
}
```

#### `onHandleNotFound()`

This function handles requests to unknown URLs, returning a 404 error.

```cpp
void onHandleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  server.send(404, "text/plain", message);
}
```

### Wi-Fi Connection Functions

#### `reconnectAPWiFi()`

This function sets the ESP8266 to AP mode and configures the AP settings.

```cpp
void reconnectAPWiFi() {
  WiFi.mode(WIFI_AP_STA); // Set mode to AP and STA
  delay(100);
  WiFi.disconnect();
  boolean status = WiFi.softAPConfig(ap_local_IP, ap_gateway, ap_subnet);
  if (status == true) {
    Serial.print("Setting soft-AP ... ");
    boolean ap = WiFi.softAP(APssid, APpass);
    if (ap == true) {
      Serial.print("connected to:\t");
      Serial.println(WiFi.softAPIP());  
    }
    server.begin(); // Start the server
  }
}
```

#### `reconnectWiFi()`

This function connects the ESP8266 to the local Wi-Fi network.

```cpp
void reconnectWiFi() {
  WiFi.mode(WIFI_STA); // Set mode to STA
  WiFi.disconnect();
   
  WiFi.begin(WiFissid, WiFipass); // Begin connection to local Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("Connected to:\t");
  Serial.println(WiFi.localIP()); // Print local IP address
}
```

## Summary

- The code initializes an ESP8266 to read temperature and humidity from an SHT31 sensor and serve this data via a web interface.
- It schedules tasks to manage reading sensor data, connecting to Wi-Fi, and serving web pages.
- The data is sent to ThingSpeak for analytics and can be viewed in real time on a web page hosted by the ESP8266.
