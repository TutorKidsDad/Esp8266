# ESP8266 Temperature Logger

This project is an ESP8266-based temperature logger that reads temperature data from a DS18x20 sensor, stores it in the SPIFFS filesystem, and provides a web interface for file management. It also retrieves time information from an NTP server.

## Features

- Reads temperature from a DS18x20 sensor.
- Logs temperature data to a CSV file in SPIFFS.
- Supports OTA (Over-The-Air) updates.
- Provides a simple web interface for file uploads and retrieval.
- Connects to multiple Wi-Fi networks.
- Retrieves and displays time from an NTP server.

## Components Required

- **ESP8266 Module** (e.g., NodeMCU, Wemos D1 Mini)
- **DS18x20 Temperature Sensor**
- **Resistor** (4.7kΩ pull-up resistor for the DS18x20 sensor)
- **Breadboard and Jumper Wires** (for connections)

## Wiring Diagram



### Connections

- **DS18x20 Temperature Sensor:**
  - VCC -> 3.3V
  - GND -> GND
  - DATA -> D5 (GPIO5)

## Libraries Required

- `OneWire`
- `DallasTemperature`
- `ESP8266WiFi`
- `ESP8266WiFiMulti`
- `WiFiUdp`
- `ArduinoOTA`
- `ESP8266WebServer`
- `ESP8266mDNS`
- `FS`

## Installation

1. **Clone the Repository:**

2. **Install Libraries:**
   Ensure the required libraries are installed via the Arduino Library Manager:
   - Open Arduino IDE
   - Go to `Sketch` -> `Include Library` -> `Manage Libraries`
   - Search for and install the libraries mentioned above.

3. **Configure Wi-Fi Credentials:**
   Open the code in the Arduino IDE and replace the placeholders in the `startWiFi()` function with your actual Wi-Fi SSIDs and passwords.

4. **Upload the Code:**
   Connect your ESP8266 to the computer and upload the code using the Arduino IDE.

## Usage

1. **Accessing the Web Interface:**
   Once uploaded, the ESP8266 will connect to the specified Wi-Fi networks. Open a web browser and navigate to the ESP8266's IP address to access the web interface.

2. **Managing Files:**
   The web interface allows you to upload files to the ESP8266's SPIFFS filesystem and view existing files.

3. **Viewing Temperature Logs:**
   The temperature data is logged to a CSV file (`/temp.csv`) in the SPIFFS filesystem. You can access this file through the web interface.

4. **OTA Updates:**
   You can update the firmware of the ESP8266 wirelessly using the OTA feature.

## Code Structure

- **Setup Functions:** Initialization of various components (Wi-Fi, OTA, SPIFFS, mDNS, HTTP server).
- **Loop Function:** Handles temperature readings, NTP time synchronization, and server requests.
- **Handler Functions:** Manage HTTP requests for file handling.
- **Helper Functions:** Support functionality like byte formatting and MIME type detection.

## Important Notes

- Ensure that the DS18x20 sensor is properly connected with a pull-up resistor.
- If the ESP8266 cannot find a DS18x20 sensor, it will reboot automatically.
- Make sure the ESP8266 has access to the internet for NTP synchronization.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Thanks to the community for the libraries and support in the ESP8266 development.

