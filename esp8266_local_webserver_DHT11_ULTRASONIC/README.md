# ESP8266 Sensor Monitoring with DHT11, HC-SR04, and I2C 16x2 LCD

This project demonstrates how to connect a DHT11 temperature and humidity sensor, an HC-SR04 ultrasonic sensor, and an I2C 16x2 LCD to an ESP8266 microcontroller. The ESP8266 creates a local web server that displays sensor data and shows the device's IP address on the LCD.

## Components Required

- ESP8266 Module (e.g., NodeMCU, Wemos D1 Mini)
- DHT11 Temperature and Humidity Sensor
- HC-SR04 Ultrasonic Sensor
- I2C 16x2 LCD
- Jumper wires
- Breadboard (optional)

## Wiring Connections

### DHT11 Sensor

| DHT11 Pin | ESP8266 Pin |
|-----------|-------------|
| VCC       | 3.3V       |
| Data      | D5 (GPIO14)|
| GND       | GND        |

### HC-SR04 Sensor

| HC-SR04 Pin | ESP8266 Pin |
|-------------|-------------|
| VCC         | 3.3V       |
| Trig        | D6 (GPIO12)|
| Echo        | D7 (GPIO13)|
| GND         | GND        |

### I2C 16x2 LCD

| LCD Pin | ESP8266 Pin |
|---------|-------------|
| VCC     | 3.3V       |
| GND     | GND        |
| SDA     | D2 (GPIO4) |
| SCL     | D1 (GPIO5) |

## Libraries Required

Make sure to install the following libraries in the Arduino IDE:

- DHT sensor library
- ESPAsyncWebServer
- LiquidCrystal_I2C
- Wire (included with Arduino IDE for I2C communication)

## Project Overview

This project does the following:

1. **Connects to Wi-Fi**: The ESP8266 connects to a specified Wi-Fi network and displays the assigned IP address on the I2C LCD.
  
2. **Reads Sensor Data**: It continuously reads temperature and humidity data from the DHT11 sensor and distance data from the HC-SR04 ultrasonic sensor.

3. **Displays Data on LCD**: Initially, the LCD shows the IP address until a web client connects. Once connected, it updates to display the current temperature and humidity.

4. **Local Web Server**: A web server runs on the ESP8266, allowing clients to access sensor data via a simple HTML interface. Users can open a web browser and enter the ESP8266's IP address to view the latest sensor readings.

## Usage

1. Update the Wi-Fi credentials in the code.
2. Upload the code to your ESP8266 using the Arduino IDE.
3. Open the Serial Monitor to see the IP address assigned to the ESP8266.
4. Connect to the IP address using a web browser to view the sensor data.
5. The LCD will show the temperature and humidity values.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgements

- ESP8266 Community
- Arduino

