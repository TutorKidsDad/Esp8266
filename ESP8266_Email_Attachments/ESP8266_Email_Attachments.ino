/*
  Rui Santos
  Complete project details at:
   - ESP8266: https://RandomNerdTutorials.com/esp8266-nodemcu-send-email-smtp-server-arduino/

  Example adapted by K. Suwatchai (Mobizt): https://github.com/mobizt/ESP-Mail-Client 
  Updated and improved by Swapnil Karambelkar
  
  This code sends an email using the ESP8266 and the ESP_Mail_Client library, with added improvements such as Wi-Fi reconnection, attachment error handling, retries, and security improvements.
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP_Mail_Client.h>
#include <LittleFS.h> // For flash memory file system

#define WIFI_SSID "REPLACE_WITH_YOUR_SSID"
#define WIFI_PASSWORD "REPLACE_WITH_YOUR_PASSWORD"

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

/* Login credentials for email */
#define AUTHOR_EMAIL "YOUR_EMAIL@XXXX.com"
#define AUTHOR_PASSWORD "YOUR_EMAIL_PASS"

/* Recipient's email */
#define RECIPIENT_EMAIL "RECIPIENT_EMAIL@XXXX.com"

/* Create an SMTPSession object for the email sending process */
SMTPSession smtp;

/* Email content stored in flash memory (PROGMEM) to save RAM */
const char emailContent[] PROGMEM = "This message contains attachments: image and text file.";

/* Function declarations */
void smtpCallback(SMTP_Status status);
bool attachFile(SMTP_Message& message, const char* filename, const char* mimeType, const char* filePath);
bool sendEmail(SMTPSession& smtp, SMTP_Message& message);
void checkWiFiConnection();

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize LittleFS for storing email attachments
  if (!LittleFS.begin()) {
    Serial.println("Failed to mount filesystem");
    return;
  }
  Serial.println("Filesystem mounted successfully");

  // Set up the email session configuration
  ESP_Mail_Session session;
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;

  // Create a new message
  SMTP_Message message;
  message.sender.name = "ESP8266 Email Sender";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "Email with Attachments";
  message.addRecipient("Recipient Name", RECIPIENT_EMAIL);

  // Add HTML message body
  message.html.content = emailContent;
  message.html.charSet = "utf-8";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_qp;

  // Add attachments (check if files exist first)
  if (!attachFile(message, "image.png", "image/png", "/image.png")) {
    Serial.println("Failed to attach image.png");
  }
  if (!attachFile(message, "text_file.txt", "text/plain", "/text_file.txt")) {
    Serial.println("Failed to attach text_file.txt");
  }

  // Set email priority and notification settings
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_normal;
  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;

  // Set the debug level (1 for basic debug)
  smtp.debug(1);

  // Set the callback function for email status
  smtp.callback(smtpCallback);

  // Try to connect and send the email
  if (!sendEmail(smtp, message)) {
    Serial.println("Failed to send the email.");
  }
}

void loop() {
  checkWiFiConnection();  // Reconnect to Wi-Fi if disconnected
  delay(60000);           // Send email every minute (adjust as needed)
}

/* Function to send an email, retrying up to 3 times if it fails */
bool sendEmail(SMTPSession& smtp, SMTP_Message& message) {
  for (int attempts = 0; attempts < 3; attempts++) {
    if (MailClient.sendMail(&smtp, &message, true)) {
      return true;
    } else {
      Serial.println("Retrying to send email...");
      delay(5000); // Wait 5 seconds before retrying
    }
  }
  return false;
}

/* Function to attach a file from LittleFS */
bool attachFile(SMTP_Message& message, const char* filename, const char* mimeType, const char* filePath) {
  if (!LittleFS.exists(filePath)) {
    Serial.println(String("File not found: ") + filePath);
    return false;
  }
  SMTP_Attachment att;
  att.descr.filename = filename;
  att.descr.mime = mimeType;
  att.file.path = filePath;
  att.file.storage_type = esp_mail_file_storage_type_flash;
  att.descr.transfer_encoding = Content_Transfer_Encoding::enc_base64;
  message.addAttachment(att);
  return true;
}

/* Function to reconnect to Wi-Fi if it gets disconnected */
void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi lost. Reconnecting...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
    }
    Serial.println("\nReconnected to Wi-Fi.");
  }
}

/* Callback function to handle email sending status */
void smtpCallback(SMTP_Status status) {
  Serial.println(status.info());  // Print the current status

  if (status.success()) {
    Serial.println("Email sent successfully.");
    Serial.println("------------------");

    // Loop through and print the results for each message sent
    struct tm dt;
    for (size_t i = 0; i < smtp.sendingResult.size(); i++) {
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);
      Serial.printf("Message No: %d\n", i + 1);
      Serial.printf("Status: %s\n", result.completed ? "success" : "failed");
      Serial.printf("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      Serial.printf("Recipient: %s\n", result.recipients);
      Serial.printf("Subject: %s\n", result.subject);
      Serial.println("------------------");
    }
  } else {
    Serial.println("Failed to send the email.");
  }
}
