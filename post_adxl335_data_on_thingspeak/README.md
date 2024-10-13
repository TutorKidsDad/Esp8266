This code reads data from the ADXL345 accelerometer sensor via I2C and sends the data over a Wi-Fi network using an ESP8266 module. The data is displayed on a webpage hosted by the ESP8266 itself. Here's a breakdown of each part of the code:

### **1. Libraries**
- `ESP8266WiFi.h`: This library allows the ESP8266 to connect to a Wi-Fi network.
- `WiFiClient.h`: Provides TCP client functionality for the ESP8266.
- `ESP8266WebServer.h`: Hosts a web server on the ESP8266 that can serve web pages.
- `Wire.h`: Used to communicate with the ADXL345 accelerometer via the I2C protocol.

### **2. Constants and Global Variables**
- `Addr`: The I2C address for the ADXL345 sensor (0x53).
- `ssid` and `password`: Wi-Fi network credentials.
- `xAccl`, `yAccl`, `zAccl`: Variables to store accelerometer data for the X, Y, and Z axes.
- `ESP8266WebServer server(80)`: Initializes a web server on port 80.

### **3. `handleroot()` Function**
This function is triggered when the web server receives a request to the root URL ("/"). It handles the following tasks:

#### I2C Communication with ADXL345
1. **Setting Registers**: 
   - Bandwidth rate register (`0x2C`) to set the output data rate to 100 Hz.
   - Power control register (`0x2D`) to disable auto-sleep mode.
   - Data format register (`0x31`) to set the sensor to full resolution mode with a ±2g range.

2. **Reading Accelerometer Data**:
   - The function reads 6 bytes of data from the sensor, representing the X, Y, and Z axis acceleration values (two bytes for each axis).

3. **Converting Data**:
   - The raw data is converted to 10-bit values.
   - If the value exceeds 511, it is adjusted for negative values (to handle signed data).

#### Serial Output
- The accelerometer readings for the X, Y, and Z axes are printed to the serial monitor for debugging and monitoring.

#### Web Server Response
- The function sends an HTML response to the client (browser) requesting the page. This HTML includes:
  - A title and a link to more information about the ADXL345 sensor.
  - The current accelerometer readings for the X, Y, and Z axes.

### **4. `setup()` Function**
1. **I2C Initialization**: Initializes the ESP8266's I2C communication on pins D4 (GPIO2) and D5 (GPIO14).
2. **Serial Communication**: Sets up serial communication at 115200 baud for debugging and output.
3. **Wi-Fi Connection**: Connects to the specified Wi-Fi network using the provided SSID and password.
   - While waiting for the connection, it prints dots to the serial monitor.
   - Once connected, it prints the IP address of the ESP8266 to the serial monitor.
4. **Web Server Setup**: 
   - Configures the server to handle requests to the root URL ("/").
   - Starts the web server.

### **5. `loop()` Function**
- Continuously checks for incoming client requests and serves the corresponding web page. If a client requests the root URL, it triggers the `handleroot()` function to display the data.

### **Applications of this Code**:
1. **IoT Projects**: This code demonstrates how to use an ESP8266 to collect sensor data and serve it over a local web server. You can monitor data from the ADXL345 accelerometer remotely using any device with a browser connected to the same network.
   
2. **Environmental Monitoring**: The accelerometer can detect changes in motion, which makes this setup useful for monitoring vibrations, tilt, or orientation of an object. For example, it could be used to detect unusual vibrations in machinery or equipment.

3. **Educational Projects**: The code is a great example of combining multiple concepts such as I2C communication, web servers, and Wi-Fi networking into one project, making it a valuable learning tool for those studying embedded systems or IoT.

4. **Data Logging**: With minor modifications, the ESP8266 could be used to send data to a cloud platform like ThingSpeak for long-term data logging and analysis.
