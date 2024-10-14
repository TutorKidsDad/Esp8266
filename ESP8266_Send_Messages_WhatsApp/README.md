
# ESP8266 WhatsApp Message Sender

This project demonstrates how to send WhatsApp messages using an ESP8266 (NodeMCU) and the [CallMeBot API](https://www.callmebot.com/).

## Requirements

- **ESP8266 (NodeMCU)** board.
- **Arduino IDE** with ESP8266 board support.
- **WiFi** credentials (SSID and Password).
- **CallMeBot API Key** for WhatsApp.

## How It Works

1. The ESP8266 connects to a WiFi network.
2. It sends a pre-defined message to a specific WhatsApp number using the [CallMeBot API](https://www.callmebot.com/).
3. If the WiFi connection drops, the ESP8266 automatically reconnects and resumes functionality.

## Setup Instructions

### 1. Install ESP8266 Board in Arduino IDE

If you haven't already set up the ESP8266 in Arduino IDE, follow these steps:

1. Open the Arduino IDE and go to **File > Preferences**.
2. In the "Additional Board Manager URLs" field, add the following URL:
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
3. Go to **Tools > Board > Board Manager** and install the "ESP8266" board.

### 2. Install Required Libraries

Ensure you have the following libraries installed in your Arduino IDE:

- `ESP8266WiFi.h`
- `ESP8266HTTPClient.h`
- `WiFiClient.h`
- `UrlEncode.h` (you can include a function or library that handles URL encoding)

### 3. Get WhatsApp API Key from CallMeBot

1. Go to [CallMeBot WhatsApp API](https://www.callmebot.com/blog/free-api-whatsapp-messages/).
2. Follow the instructions to get your WhatsApp API key.
3. Save your **phone number** and **API key** for later.

### 4. Update the Code

1. Open the `ESP8266` code file in Arduino IDE.
2. Replace the placeholders in the code with your actual data:
   ```cpp
   const char* ssid = "REPLACE_WITH_YOUR_SSID";
   const char* password = "REPLACE_WITH_YOUR_PASSWORD";
   String phoneNumber = "REPLACE_WITH_YOUR_PHONE_NUMBER";  // e.g., "+351912345678"
   String apiKey = "REPLACE_WITH_API_KEY";  // Your CallMeBot API key
   ```
   
### 5. Upload the Code

1. Connect your ESP8266 board to your computer.
2. Select your board and the correct port under **Tools > Board** and **Tools > Port**.
3. Click **Upload** to flash the code to your ESP8266.

### 6. Monitor the Serial Output

Open the **Serial Monitor** in Arduino IDE (set baud rate to `115200`) to check for connection status and message-sending logs.

### 7. Testing

Once the ESP8266 is connected to your WiFi, it will send a WhatsApp message to the specified number. The message will say:

```
Hello from ESP8266!
```

You can modify the message content by changing the string passed to the `sendMessage()` function.

### 8. Troubleshooting

- **WiFi Connection Issues**: Ensure your SSID and password are correct. If the ESP8266 is unable to connect to WiFi, it will retry for a set number of attempts (30).
- **API Errors**: If the API request fails, check the API key and phone number format. The HTTP response code will be printed to the Serial Monitor for debugging.

## License

This project is based on the example provided by Rui Santos, licensed under the MIT License. You are free to use and modify the project as needed.

