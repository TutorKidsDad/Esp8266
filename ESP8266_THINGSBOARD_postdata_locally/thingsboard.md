### Installing and Connecting to Thingsboard

**Thingsboard** is an open-source platform for data collection, processing, visualization, and device management. To use Thingsboard with your ESP8266 (or any IoT device), you need to install it on a server, configure it, and establish communication via MQTT.

Here’s how to install Thingsboard and connect your ESP8266 to it:

### 1. Installing Thingsboard

There are multiple ways to install Thingsboard depending on your environment (local machine, cloud, Docker, etc.). The most common approach is to install Thingsboard on a Linux server using Docker or by manually installing it.

#### A. Install Thingsboard Using Docker (Recommended)
1. **Install Docker**:
   - For Ubuntu:
     ```bash
     sudo apt update
     sudo apt install docker.io
     sudo systemctl start docker
     sudo systemctl enable docker
     ```
   
   - For other systems, follow the Docker [installation guide](https://docs.docker.com/get-docker/).

2. **Install Thingsboard using Docker**:
   - Pull the Thingsboard Docker image:
     ```bash
     docker pull thingsboard/tb-postgres
     ```

   - Run Thingsboard with PostgreSQL database:
     ```bash
     docker run -it -d --name tb -p 8080:8080 -p 1883:1883 -p 5683:5683/udp thingsboard/tb-postgres
     ```

3. **Access Thingsboard**:
   - After installation, Thingsboard will run on port **8080**.
   - Open a browser and navigate to `http://<server-ip>:8080/` (for local setup, use `localhost`).
   - Default login credentials:
     - **Username**: `tenant@thingsboard.org`
     - **Password**: `tenant`

#### B. Install Thingsboard Manually
1. **Install Java** (Thingsboard requires Java 11):
   - For Ubuntu:
     ```bash
     sudo apt update
     sudo apt install openjdk-11-jdk
     ```

2. **Install PostgreSQL**:
   ```bash
   sudo apt install postgresql postgresql-contrib
   ```

3. **Install Thingsboard**:
   - Download Thingsboard and follow the official [manual installation guide](https://thingsboard.io/docs/user-guide/install/ubuntu/).

4. **Start Thingsboard**:
   ```bash
   sudo service thingsboard start
   ```

5. **Access the dashboard** using `http://<server-ip>:8080/`.

---

### 2. Configuring Thingsboard

Once you’ve installed Thingsboard, you need to configure it to accept data from your ESP8266.

#### Step 1: Create a Device on Thingsboard

1. **Login** to Thingsboard using the default credentials (`tenant@thingsboard.org` / `tenant`).
2. **Navigate to the Devices section**:
   - Click on the **Devices** tab on the left-hand side.
   - Click the "+" button to add a new device.
   - Give the device a name (e.g., **ESP8266 Sensor**).
   - Optionally, select the device type (e.g., **default**).
3. **Save the device** and open it.
4. Go to the **Credentials** tab.
   - You'll see a **Device Token** generated automatically. Copy this token, as you’ll need it to connect your ESP8266.

#### Step 2: Enable MQTT Integration (Optional)
Thingsboard supports MQTT by default, but you can enable extra MQTT settings if needed.

1. Go to the **Settings** tab.
2. Under **MQTT** settings, you can configure the host, ports, and security.

### 3. Connecting ESP8266 to Thingsboard

You can now configure your ESP8266 to send data to Thingsboard using the MQTT protocol.

#### Step 1: Install Necessary Libraries in Arduino IDE
In your **Arduino IDE**, install the following libraries:
1. **PubSubClient** (for MQTT communication).
2. **DHT** (for DHT sensor data).
3. **ESP8266WiFi** (for ESP8266 Wi-Fi functionality).

#### Step 2: Configure Your Sketch
Modify your Arduino code to connect to Thingsboard:

1. **Wi-Fi Credentials**: Set your Wi-Fi name (`SSID`) and password.
2. **Thingsboard Token**: Use the token copied from Thingsboard’s device credentials.

### 4. Monitoring Data on Thingsboard

1. **Login** to Thingsboard and go to your **ESP8266 device**.
2. Navigate to the **Latest Telemetry** tab to see real-time data sent by your ESP8266 (temperature and humidity).
3. Optionally, create **dashboards** to visualize your data with widgets like graphs or gauges.

### Summary:
1. Install Thingsboard (Docker or manually).
2. Create a device and obtain its token.
3. Use Arduino IDE to configure your ESP8266 to read data from sensors (like DHT11) and send it to Thingsboard via MQTT.
4. Monitor the data on the Thingsboard dashboard.


