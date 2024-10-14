
# BMP388 Web Server using ESP8266

This project creates a web server on an ESP8266 microcontroller that displays real-time temperature, pressure, and altitude readings from the BMP388 sensor. The data is automatically updated using Server-Sent Events (SSE).

## Features

- Connects to your Wi-Fi network.
- Serves a web page with real-time data from the BMP388 sensor.
- Automatically updates the web page with the latest sensor readings every 30 seconds.
- Provides readings for:
  - **Temperature** (°C)
  - **Pressure** (hPa)
  - **Altitude** (m)

## Hardware Required

1. **ESP8266** (NodeMCU or any ESP8266-based development board)
2. **BMP388 Sensor**
3. Jumper wires
4. Breadboard (optional)

## Software Requirements

1. **Arduino IDE** (with ESP8266 board support installed)
2. Required libraries:
   - `ESPAsyncWebServer` - for handling web server requests.
   - `Adafruit BMP3XX` - for BMP388 sensor communication.
   - `ESP8266WiFi` - for Wi-Fi connectivity.
   - `Adafruit Sensor` - for sensor abstractions.

## Circuit Diagram

Connect your ESP8266 to the BMP388 sensor as follows:

| **BMP388 Pin** | **ESP8266 Pin** |
|:--------------:|:---------------:|
|       VIN      |       3.3V      |
|       GND      |       GND       |
|       SCL      |       D1 (GPIO5)|
|       SDA      |       D2 (GPIO4)|

## Setup Instructions

### 1. Install Libraries
Install the following libraries in the Arduino IDE:

- `Adafruit Sensor` library
- `Adafruit BMP3XX` library
- `ESPAsyncWebServer` library

### 2. Uploading the Code
- Open the Arduino IDE.
- Install the required libraries using the Arduino Library Manager.
- Copy the provided code into the Arduino IDE.
- Replace the placeholders in the code with your Wi-Fi credentials:
  ```cpp
  const char* ssid = "REPLACE_WITH_YOUR_SSID";
  const char* password = "REPLACE_WITH_YOUR_PASSWORD";
  ```
- Upload the code to your ESP8266 board.

### 3. Access the Web Server
- After uploading the code, open the Serial Monitor at a baud rate of 115200.
- Wait for the ESP8266 to connect to Wi-Fi.
- Once connected, the ESP8266 will print the IP address on the Serial Monitor:
  ```
  Connected! IP Address: 192.168.x.x
  ```
- Open your web browser and enter the IP address shown in the Serial Monitor. You should see the web page displaying real-time sensor readings.

### 4. Data Update
- The sensor readings (temperature, pressure, altitude) automatically refresh every 30 seconds.
  
## Customization

- You can adjust the **timerDelay** variable in the `loop()` function to change the update interval:
  ```cpp
  unsigned long timerDelay = 30000;  // Time in milliseconds (30 seconds)
  ```

## Troubleshooting

- Ensure the BMP388 sensor is properly wired to the ESP8266.
- If the sensor isn't detected, check the connections and the power supply.
- If Wi-Fi connection fails, the ESP8266 will attempt to reconnect after a restart.

## License

This project is open-source and available under the MIT License. See the original project details by [Rui Santos](https://RandomNerdTutorials.com/esp8266-nodemcu-bmp388-arduino/).

