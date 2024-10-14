
# ESP8266 Wi-Fi Configuration Portal

This project provides a user-friendly interface for configuring Wi-Fi settings on an ESP8266 microcontroller without the need to hard-code credentials. It establishes an access point (AP) that hosts a web portal, allowing users to input their Wi-Fi SSID and password, which are then stored in the device's EEPROM for future use.

## Features

- **Dynamic Wi-Fi Configuration:** Enables users to set or update Wi-Fi credentials at runtime without modifying the code.
- **Web Portal Interface:** Hosts a web page where users can enter their Wi-Fi details.
- **EEPROM Storage:** Securely stores the provided Wi-Fi credentials in EEPROM for persistent use.

## How It Works

1. **Startup:** Upon power-up, the ESP8266 attempts to connect to the Wi-Fi network using credentials stored in EEPROM.
2. **Connection Check:** If the connection is unsuccessful (e.g., due to incorrect or missing credentials), the device initiates its own AP.
3. **User Configuration:** Users can connect to this AP using a smartphone or computer to access the configuration portal.
4. **Credential Entry:** Within the portal, users input their home Wi-Fi SSID and password.
5. **Storage:** The device stores these credentials in EEPROM.
6. **Reconnection:** The ESP8266 restarts and attempts to connect to the specified Wi-Fi network using the newly provided credentials.

## Setup Instructions

1. **Hardware Setup:** Connect the ESP8266 to your computer using a USB cable.
2. **Software Requirements:** Ensure you have the Arduino IDE installed with the necessary ESP8266 board support.
3. **Code Upload:** Upload the provided code to the ESP8266 using the Arduino IDE.
4. **Initial Configuration:** After uploading, the ESP8266 will reboot. If it cannot connect to the Wi-Fi network, it will create an AP (e.g., named "ESP8266_Config"). Connect to this network to access the configuration portal.
5. **Enter Credentials:** Once connected, navigate to the portal's IP address (commonly 192.168.4.1) in a web browser to enter your home Wi-Fi SSID and password.
6. **Save and Reboot:** Save the settings. The ESP8266 will reboot and attempt to connect to your Wi-Fi network using the provided credentials.

## Dependencies

- **Libraries:** The code utilizes the `ESP8266WiFi.h`, `ESP8266WebServer.h`, and `EEPROM.h` libraries, which are typically included in the ESP8266 core for Arduino.

## Notes

- **Security:** Ensure that the configuration portal is secured to prevent unauthorized access to your Wi-Fi credentials. Implementing a password for the AP or using other security measures is recommended.
- **Persistence:** The credentials are stored in EEPROM, which has a limited number of write cycles. Avoid frequent updates to prolong the EEPROM's lifespan.

For more detailed information and troubleshooting, refer to the project's GitHub repository.
