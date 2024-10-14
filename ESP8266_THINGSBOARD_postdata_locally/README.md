This code is for an **ESP8266** microcontroller that uses a **DHT11 sensor** to collect temperature and humidity data. It connects to a **Wi-Fi network** and sends the sensor data to a **Thingsboard** server using **MQTT** protocol.

Here's a detailed explanation of what each part does:

### Libraries Used:
1. **DHT.h**: Library to interface with the DHT11 sensor for reading temperature and humidity.
2. **PubSubClient.h**: Library for MQTT communication, allowing the ESP8266 to publish data to the Thingsboard server.
3. **ESP8266WiFi.h**: Library to manage Wi-Fi connections.

### Defined Constants:
- **WIFI_AP** and **WIFI_PASSWORD**: The name and password of the Wi-Fi network the ESP8266 will connect to (`"OnlineTPS"` and `"TPS@462014"`).
- **TOKEN**: Authentication token for the Thingsboard server, used for identifying the device.
- **DHTPIN**: Pin where the DHT11 sensor is connected (`D4`).
- **DHTTYPE**: Type of DHT sensor used, in this case, `DHT11`.
- **thingsboardServer**: The IP address of the Thingsboard server to which data will be sent (`192.168.1.11`).

### Variables:
- `wifiClient`: The ESP8266's Wi-Fi client object.
- `dht`: Object to interact with the DHT11 sensor.
- `client`: MQTT client for communication with Thingsboard.
- `status`: Stores the Wi-Fi connection status.
- `lastSend`: Tracks the last time data was sent, used for throttling data transmission to Thingsboard.

### Functions:

#### `setup()`
This function runs once when the ESP8266 starts:
1. Initializes the serial communication (`Serial.begin(115200)`).
2. Starts the DHT sensor (`dht.begin()`).
3. Connects to the Wi-Fi network using `InitWiFi()`.
4. Configures the MQTT client to communicate with Thingsboard at port 1883 (`client.setServer( thingsboardServer, 1883 )`).
5. Initializes `lastSend` to 0.

#### `loop()`
This function runs repeatedly:
1. Checks if the MQTT client is connected; if not, it calls `reconnect()` to restore the connection.
2. Checks if at least 1 second has passed since the last data transmission. If so, it reads the temperature and humidity data and sends it to Thingsboard by calling `getAndSendTemperatureAndHumidityData()`.
3. Keeps the MQTT connection alive by calling `client.loop()`.

#### `getAndSendTemperatureAndHumidityData()`
This function collects data from the DHT11 sensor and sends it to the Thingsboard server:
1. Reads humidity (`dht.readHumidity()`) and temperature (`dht.readTemperature()`) from the sensor.
2. If the sensor fails to read data, it logs an error.
3. Converts the readings into `String` objects.
4. Constructs a JSON-formatted payload with the temperature and humidity readings.
5. Sends the payload using MQTT (`client.publish()`).

#### `InitWiFi()`
This function establishes a Wi-Fi connection:
1. Initiates the connection using the provided Wi-Fi credentials.
2. Waits in a loop until the connection is established, indicated by `WiFi.status()` becoming `WL_CONNECTED`.

#### `reconnect()`
This function attempts to reconnect to the Thingsboard server if the MQTT connection is lost:
1. If the ESP8266 is disconnected from Wi-Fi, it reconnects using `WiFi.begin()`.
2. Then it attempts to connect to Thingsboard using the device token for authentication (`client.connect("ESP8266 Device", TOKEN, NULL)`).
3. If the connection fails, it waits 5 seconds before retrying.

### How It Works:
- The ESP8266 connects to the Wi-Fi and establishes a connection to Thingsboard using MQTT.
- It reads the temperature and humidity from the DHT11 sensor every second.
- The data is formatted as a JSON payload and sent to the Thingsboard server, where it can be visualized or processed further.
- If the MQTT or Wi-Fi connection is lost, the ESP8266 attempts to reconnect automatically.

This system enables real-time monitoring of temperature and humidity from a remote server.
