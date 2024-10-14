
# ESP8266 Email Sender with Attachments

This project demonstrates how to send emails with attachments using an ESP8266 and the ESP_Mail_Client library. The code allows the ESP8266 to send emails through an SMTP server (e.g., Gmail) with image and text file attachments from the ESP's flash storage.

## Features

- Connects to Wi-Fi and sends an email with attachments.
- Attachments are stored in the ESP8266's flash memory (`LittleFS`).
- Includes retry logic for failed email sends.
- Handles Wi-Fi reconnection in case of network drops.
- Debugging information and detailed email status logging.

## Prerequisites

### Hardware

- **ESP8266 (NodeMCU, Wemos D1, etc.)**
- **Micro USB cable**
- **Computer with Arduino IDE**

### Libraries

Before uploading the code, ensure you have the following libraries installed in the Arduino IDE:

- [ESP8266 Core for Arduino](https://github.com/esp8266/Arduino)
- [ESP_Mail_Client](https://github.com/mobizt/ESP-Mail-Client)
- [LittleFS](https://github.com/esp8266/arduino-esp8266fs-plugin) (for file storage on flash memory)

### Arduino IDE Setup

1. Install the **ESP8266 Core**:
   - Open the Arduino IDE.
   - Go to `File > Preferences`.
   - Add the following URL to the **Additional Boards Manager URLs**: 
     ```
     http://arduino.esp8266.com/stable/package_esp8266com_index.json
     ```
   - Go to `Tools > Board > Boards Manager`.
   - Search for **ESP8266** and install the board package.

2. Install the **ESP_Mail_Client** library:
   - Go to `Sketch > Include Library > Manage Libraries`.
   - Search for **ESP_Mail_Client** and install it.

3. Install the **LittleFS Plugin**:
   - Download the LittleFS plugin for your system from [here](https://github.com/esp8266/arduino-esp8266fs-plugin).
   - Install the plugin in the Arduino IDE to manage files in the ESP8266's flash memory.

### Flash File System (LittleFS)

To store attachments (like images or text files), the ESP8266 uses the `LittleFS` file system. You need to upload the files to the ESP8266 before running the code:

1. **Prepare your files**: Save your image (`image.png`) and text file (`text_file.txt`) in the `data/` folder in your project directory.
2. **Upload files to ESP8266**: Use the LittleFS upload tool in the Arduino IDE (after installing the LittleFS plugin) to upload these files to the ESP8266's flash memory.

## Configuration

### Wi-Fi Credentials

In the code, replace the following placeholders with your actual Wi-Fi credentials:

```cpp
#define WIFI_SSID "REPLACE_WITH_YOUR_SSID"
#define WIFI_PASSWORD "REPLACE_WITH_YOUR_PASSWORD"
```

### Email Credentials

Replace the following placeholders with your actual Gmail credentials:

```cpp
#define AUTHOR_EMAIL "YOUR_EMAIL@XXXX.com"
#define AUTHOR_PASSWORD "YOUR_EMAIL_PASS"
```

> **Note:** If you're using Gmail, you'll need to generate an **App Password** for this to work. Follow the instructions from [Google](https://support.google.com/accounts/answer/185833?hl=en) to create one.

### Recipient Email

Replace the recipient's email address with your own or the intended recipient's email:

```cpp
#define RECIPIENT_EMAIL "RECIPIENT_EMAIL@XXXX.com"
```

## Usage

1. **Upload your files**: Use the LittleFS uploader tool to upload `image.png` and `text_file.txt` to the ESP8266.
2. **Upload the sketch**: Once your files are uploaded, upload the provided Arduino sketch to the ESP8266.
3. **Monitor the serial output**: Open the Serial Monitor (`Ctrl + Shift + M`) in the Arduino IDE and set the baud rate to `115200`. You should see debug information showing Wi-Fi connection status, file attachment status, and the email sending process.
4. **Success**: If the email sends successfully, it will appear in the recipient's inbox with the attached image and text file.

## Code Structure

- **setup()**: Initializes Wi-Fi, mounts the filesystem, sets up the SMTP session, and attempts to send the email.
- **loop()**: Handles Wi-Fi reconnection and periodic email sending (every minute).
- **sendEmail()**: Manages the email sending process, with retries in case of failure.
- **attachFile()**: Attaches files from flash memory to the email.
- **smtpCallback()**: Provides feedback on the status of the email sending process.

## Debugging

The code provides basic debugging through the serial monitor. If an email fails to send, check the output for specific errors related to Wi-Fi, file handling, or email server issues.

## License

This project is licensed under the **MIT License**. See the original example adapted from Rui Santos and Mobizt.

