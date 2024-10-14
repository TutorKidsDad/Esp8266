
# ESP8266 Temperature Monitoring with DS18B20 Sensors and Web Interface

This project uses the ESP8266 (or NodeMCU) to read temperature values from multiple DS18B20 sensors, serve the data via a web interface, and visualize it in real-time using Chart.js. The sensor data is accessible through a web browser and updates every 10 seconds.

## Features
- Real-time temperature readings from up to four DS18B20 sensors.
- Data is served via a web page hosted on the ESP8266.
- Chart.js is used to visualize temperature readings on the web interface.
- Automatic WiFi reconnection in case of connection issues.

## Components Required
- **ESP8266** or **NodeMCU**
- **DS18B20** Temperature Sensors (up to four)
- **4.7kΩ Resistor** (for DS18B20 sensors)
- Jumper wires
- Breadboard

## Circuit Diagram
1. Connect the **VCC** pin of the DS18B20 to the **3.3V** pin on the ESP8266.
2. Connect the **GND** pin of the DS18B20 to the **GND** pin of the ESP8266.
3. Connect the **Data** pin of the DS18B20 to **GPIO 4 (D2)** of the ESP8266.
4. Place the **4.7kΩ** resistor between the **Data** and **VCC** pins of the DS18B20 sensor.

### Circuit Diagram (Simple Line Drawing)
```
  ESP8266         DS18B20
    VCC  ------>  VCC
    GND  ------>  GND
   GPIO4 ------>  DATA
               (4.7kΩ resistor between DATA and VCC)
```

## Installation

### 1. Software Requirements
- **Arduino IDE** with the **ESP8266** board package installed.
- Required libraries (install via Arduino Library Manager):
  - **ESPAsyncWebServer**
  - **Arduino_JSON**
  - **DallasTemperature**
  - **OneWire**
  - **LittleFS_esp8266**

### 2. Clone the Project
Clone this repository or download the source files.

```bash
git clone https://github.com/yourusername/esp8266-temperature-monitoring.git
```

### 3. Upload Files to LittleFS
Ensure you upload the `index.html` file to the ESP8266's LittleFS. You can do this by using the **ESP8266 LittleFS Data Upload Tool** available for Arduino IDE.

### 4. Configure WiFi
Replace the placeholders in the code (`ssid` and `password`) with your WiFi credentials:

```cpp
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
```

### 5. Upload the Code
- Connect your ESP8266 to your computer.
- Open the `.ino` file in the Arduino IDE.
- Select the correct board and port.
- Upload the code to the ESP8266.

### 6. Access the Web Interface
After uploading the code, open the Serial Monitor (set the baud rate to 115200) to see the assigned IP address of the ESP8266. Open a web browser and navigate to that IP address to access the real-time temperature readings and chart.

## File Structure

```bash
├── esp8266-temperature-monitoring.ino  # Main Arduino sketch
├── data
│   └── index.html                      # Web interface file (HTML + Chart.js)
└── README.md                           # Project description
```

## How It Works
1. **Temperature Reading**: The ESP8266 reads temperature values from four DS18B20 sensors connected via GPIO 4. It uses the **OneWire** and **DallasTemperature** libraries for sensor communication.
   
2. **Web Server**: An asynchronous web server is created using the **ESPAsyncWebServer** library. The server hosts a web page (`index.html`) stored in the LittleFS filesystem of the ESP8266.
   
3. **Real-time Updates**: The web interface uses **Chart.js** to display temperature data in a line chart. The data is updated every 10 seconds by making asynchronous requests to the ESP8266 server.

4. **WiFi Reconnection**: If the WiFi connection drops, the ESP8266 attempts to reconnect automatically, ensuring the server remains accessible.

## Web Interface
Once the ESP8266 is connected to WiFi, you can view the real-time temperature data by visiting the IP address assigned to the ESP8266 in your browser. The page includes:
- **A chart** displaying live temperature readings from the sensors.
- **Automatic updates** every 10 seconds.

### Sample Screenshot
(Insert a sample screenshot of the web interface here, showing the temperature graph)

## Troubleshooting

### 1. Serial Monitor shows "Failed to mount LittleFS"
- Ensure the **LittleFS** library is installed and the data folder is uploaded correctly using the **LittleFS Data Upload Tool**.

### 2. "Error" in Temperature Readings
- Check if all DS18B20 sensors are connected properly and ensure the pull-up resistor (4.7kΩ) is in place.

### 3. Unable to Connect to WiFi
- Double-check the WiFi credentials in the code.
- Make sure the ESP8266 is within range of the WiFi router.

### 4. Web Interface Not Loading
- Ensure the IP address in the browser matches the one printed in the Serial Monitor.
- Check that the ESP8266 is connected to the correct WiFi network.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

