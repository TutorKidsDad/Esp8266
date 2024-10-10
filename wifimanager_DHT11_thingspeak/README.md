# ESP8266 MQTT DHT11 Sensor with ThingSpeak

This project uses an ESP8266 to read temperature and humidity data from a DHT11 sensor and publish it to ThingSpeak via MQTT. The code connects to Wi-Fi, reads sensor data, and sends the data to a specified ThingSpeak channel using MQTT. Wi-Fi credentials are handled using WiFiManager, allowing auto-connect or AP mode for Wi-Fi setup.

## Prerequisites

1. **Hardware Required:**
   - ESP8266 board (NodeMCU or similar)
   - DHT11 Temperature and Humidity Sensor
   - Jumper wires and breadboard

2. **Software Required:**
   - Arduino IDE
   - ESP8266 Board Package
   - Required Libraries: 
     - ESP8266WiFi
     - PubSubClient
     - DHT
     - WiFiManager

## Installation and Setup

1. **Install ESP8266 Board Package:**
   - Open Arduino IDE and go to `File > Preferences`.
   - Add the following URL in the "Additional Board Manager URLs": 
     ```
     http://arduino.esp8266.com/stable/package_esp8266com_index.json
     ```
   - Go to `Tools > Board > Board Manager`, search for "ESP8266", and install the package.

2. **Install Libraries:**
   - Go to `Sketch > Include Library > Manage Libraries` and search for and install the following:
     - `PubSubClient`
     - `DHT sensor library`
     - `WiFiManager`

3. **Wiring:**
   - Connect the DHT11 sensor to your ESP8266 as follows:
     - **VCC** to **3.3V**
     - **GND** to **GND**
     - **DATA** to **D3**

4. **MQTT and ThingSpeak Configuration:**
   - Create a ThingSpeak account at [ThingSpeak](https://thingspeak.com/).
   - Create a new channel and note your **Channel ID** and **Write API Key**.
   - Set up the ThingSpeak MQTT credentials:
     - **MQTT Server:** `mqtt.thingspeak.com`
     - **MQTT Topic:** `channels/<ChannelID>/publish/<APIKey>`
     - Replace `ChannelID` and `APIKey` with your actual values in the code.

## Code Overview

### Libraries

- **ESP8266WiFi**: Manages Wi-Fi connections.
- **PubSubClient**: Used for MQTT communication with ThingSpeak.
- **WiFiManager**: Helps manage Wi-Fi credentials and configuration.
- **DHT**: Manages the DHT11 sensor for reading temperature and humidity.

### Constants

- `MQTT_USER_KEY`: Your MQTT User Key (typically not needed with ThingSpeak, so you can leave this blank).
- `CHANNEL_ID`: Your ThingSpeak channel ID.
- `CHANNEL_API_KEY_WR`: Your ThingSpeak Write API Key.
- `MQTT_SERVER`: MQTT server for ThingSpeak (`mqtt.thingspeak.com`).
- `DHTPIN`: Pin where the DHT11 is connected (`D3` in this case).

### Functions

1. **`mqttConnect()`**: Attempts to connect to the MQTT broker, retrying up to 10 times with a randomly generated client ID.
2. **`mqttPublish(String msg)`**: Publishes data to the MQTT broker.
3. **`setup()`**:
   - Initializes the serial communication.
   - Starts the DHT sensor.
   - Manages Wi-Fi connection using WiFiManager.
   - Sets the MQTT server details.
4. **`loop()`**:
   - Reads temperature and humidity from the DHT11 sensor.
   - Publishes the data to the MQTT broker at intervals of 15 seconds.
   - Reconnects to the MQTT broker if the connection is lost.

### Sample MQTT Message

The message sent to ThingSpeak is formatted as:
```
<APIKey>&field1=<temperature>&field2=<humidity>
```

## How It Works

1. On startup, the ESP8266 tries to connect to Wi-Fi using saved credentials or opens an AP if none are available.
2. The DHT11 sensor continuously reads temperature and humidity values.
3. The sensor data is sent to ThingSpeak via MQTT every 15 seconds.
4. WiFiManager simplifies the Wi-Fi setup process by opening an AP mode if Wi-Fi credentials are missing.

## Usage

1. Flash the code onto your ESP8266.
2. The device will either connect to the saved Wi-Fi network or enter AP mode (SSID: `ESP8266_MQTT_AP`) to configure the Wi-Fi.
3. Data from the DHT11 sensor will automatically be published to your ThingSpeak channel every 15 seconds.
4. You can monitor the data in real-time by accessing your ThingSpeak channel.

## Troubleshooting

- If you encounter connection issues with the MQTT server, make sure that:
  - The Wi-Fi credentials are correct.
  - The ThingSpeak Channel ID and Write API Key are valid.
- Ensure the DHT sensor is connected correctly and functioning.

### Dependencies
- `ESP8266WiFi.h`: Manages Wi-Fi connectivity.
- `PubSubClient.h`: Manages MQTT client functionality.
- `DHT.h`: Provides support for the DHT sensor.
- `WiFiManager.h`: Allows easy setup of Wi-Fi credentials.


