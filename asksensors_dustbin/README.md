
# Ultrasonic Smart Dustbin with WiFi and ASKSENSORS Integration

This project is an IoT-based smart dustbin using an ultrasonic sensor and an ESP8266 WiFi module. The ultrasonic sensor detects the fill level of the dustbin, and the data is sent to a cloud-based API (ASKSENSORS) via WiFi. The system reports the distance between the dustbin lid and the waste level, allowing you to monitor how full the dustbin is in real-time.

## Components

- **ESP8266 NodeMCU**
- **Ultrasonic Sensor (HC-SR04)**
- **WiFi Connectivity**
- **ASKSENSORS API for data logging**

## Features

- Measure the fill level of the dustbin using the ultrasonic sensor.
- Send the fill level percentage to the ASKSENSORS cloud.
- WiFi reconnection to ensure data reliability.
- 20-second delay between each reading and data transmission.

## Requirements

### Hardware:

1. **ESP8266 (NodeMCU)**
2. **Ultrasonic Sensor (HC-SR04)**
3. **Wires and Breadboard**
4. **Power Source for ESP8266**

### Software:

1. **Arduino IDE** with ESP8266 Board Support
2. **Libraries**:
   - ESP8266WiFi
   - ESP8266HTTPClient
3. **ASKSENSORS Account** for API key

## Circuit Diagram

- **Ultrasonic Sensor:**
  - Trig pin: D1 (GPIO 5)
  - Echo pin: D2 (GPIO 4)
- **ESP8266:** 
  - Connect ESP8266 to the ultrasonic sensor via GPIO pins.

## Configuration

1. Sign up at [ASKSENSORS](https://asksensors.com) to obtain your **API Key**.
2. In the code, update the following with your WiFi credentials and ASKSENSORS API Key:

   ```cpp
   const char* wifi_ssid = "your_wifi_ssid";             
   const char* wifi_password = "your_wifi_password";         
   const char* apiKeyIn = "your_asksensors_api_key";
   ```

## Installation

1. Install the [Arduino IDE](https://www.arduino.cc/en/Main/Software) if you haven't already.
2. Install the **ESP8266** board in the Arduino IDE by going to **File -> Preferences**, and adding this URL to the "Additional Boards Manager URLs" section: `http://arduino.esp8266.com/stable/package_esp8266com_index.json`.
3. Install the required libraries:
   - ESP8266WiFi
   - ESP8266HTTPClient
4. Connect the ESP8266 board to your PC via USB.

## Code

The full source code for the project is provided in the `src` folder. Here's a quick overview of what the code does:

1. Connects to WiFi using the provided SSID and password.
2. Measures the distance using the ultrasonic sensor.
3. Converts the distance to a fill-level percentage.
4. Sends the data to ASKSENSORS using an HTTP GET request.
5. Repeats every 20 seconds.

## Uploading the Code

1. Open the Arduino IDE.
2. Select the ESP8266 board from **Tools -> Board -> NodeMCU 1.0 (ESP-12E Module)**.
3. Paste the provided code into the editor.
4. Update the WiFi and ASKSENSORS configuration.
5. Upload the code to your ESP8266 using **Sketch -> Upload**.

## Usage

1. After uploading the code to the ESP8266, open the **Serial Monitor** (Ctrl + Shift + M) to check if the device is connected to WiFi and sending data.
2. You should see the distance and mapped fill percentage printed in the Serial Monitor.
3. The fill-level data will be logged in your ASKSENSORS account under the provided API Key.

## Troubleshooting

- **WiFi Connection Issues**: Ensure that the SSID and password are correct, and the router is within range.
- **Incorrect Distance Readings**: Check the wiring of the ultrasonic sensor and ensure the `TRIG` and `ECHO` pins are connected correctly.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.


