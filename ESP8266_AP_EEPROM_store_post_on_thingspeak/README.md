This program is designed for logging temperature data from DS18B20 temperature sensors using an ESP8266 microcontroller (NodeMCU or D1 Mini). It can work in two modes: Access Point (AP) mode or Station mode. The program logs data locally and can send the temperature readings to ThingSpeak, a cloud platform, for remote monitoring. Here's an overview of the main components and functions:

### Libraries
- **OneWire**, **DallasTemperature**: Used for interfacing with DS18B20 temperature sensors.
- **ESP8266WiFi**, **ESP8266WebServer**: Handle WiFi connections and create a web server for configuration and data access.
- **EEPROM**: Store configuration data such as WiFi credentials and API keys for ThingSpeak.
- **WiFiUdp**: For synchronizing time with an NTP (Network Time Protocol) server.

### Definitions and Constants
- **ONE_WIRE_BUS**: Pin where the DS18B20 data line is connected (D4).
- **EE_CLEAR_BUTTON**: Button (D3) to clear EEPROM data (WiFi credentials, settings).
- **EEPROM Indices**: Constants used for storing data in specific EEPROM addresses (e.g., SSID, password, ThingSpeak API key).
- **Intervals**: Defines how often the system will ask the temperature sensors (DS18B20_CYCLE_SEC) and request time updates from the NTP server (NTP_CYCLE_MIN).

### WiFi and Mode Setup
- **AP Mode**: If no WiFi credentials are stored in EEPROM, the system starts in Access Point mode. The ESP8266 creates a network where the user can connect, access a web interface, and configure WiFi settings.
- **Station Mode**: If valid credentials are stored, the device connects to the local WiFi network in Station mode. It retrieves temperatures, sends them to ThingSpeak, and synchronizes time with an NTP server.

### Main Functions
1. **`Read_EE()`**: Reads stored WiFi settings and ThingSpeak configurations from EEPROM. If no data is found, it clears EEPROM and enters AP mode.
2. **`init_DS18B20()`**: Initializes and locates the connected DS18B20 sensors. It checks if the expected number of sensors are found and prints their addresses.
3. **`read_DS18B20()`**: Requests temperature data from the sensors. If a valid reading is received, the values are stored, and an average is calculated.
4. **`setupAP()`**: Sets up the device in Access Point mode, allowing users to connect to a custom WiFi network and configure the device via a web interface.
5. **`setupST()`**: Starts Station mode by connecting to the local WiFi using the credentials saved in EEPROM.
6. **`APwebServer()` and `STwebServer()`**: Handle the web interface. In AP mode, the user can enter WiFi settings; in Station mode, the device displays the current temperature readings and ThingSpeak field values.
7. **`buttonPress()`**: Monitors the button to reset the EEPROM. If pressed for a certain time, it will clear the stored configuration and restart the device.

### Data Logging and Sending
- **NTP Time Sync**: The program periodically syncs with an NTP server to get the current time.
- **ThingSpeak**: If in Station mode and configured properly, temperature data from each sensor is sent to ThingSpeak at regular intervals.

### EEPROM Structure
The EEPROM is used to store:
- SSID, password, hostname, ThingSpeak API key, and other configuration data.
- Each piece of information is stored in a predefined index, making it easy to retrieve and write during configuration.

### Program Flow
1. At startup, the program checks if configuration data exists in EEPROM:
   - If it does, the device enters Station mode and connects to WiFi.
   - If not, it starts in AP mode, where the user can set up the device.
2. Temperature readings are taken from the DS18B20 sensors at regular intervals.
3. In Station mode, data is sent to ThingSpeak, and the time is synchronized with an NTP server.

This system is versatile for temperature monitoring, allowing easy configuration via a web interface, and offers remote logging through ThingSpeak.
