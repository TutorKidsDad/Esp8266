#include <Wire.h>
#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include "config.h"  // Configuration file for sensitive data

const uint8_t MPU6050SlaveAddress = 0x68;  // MPU6050 I2C address

const uint8_t scl = D1;  // SCL pin
const uint8_t sda = D2;  // SDA pin

const uint16_t AccelScaleFactor = 16384;
const uint16_t GyroScaleFactor = 131;

const uint8_t MPU6050_REGISTER_SMPLRT_DIV   =  0x19;
const uint8_t MPU6050_REGISTER_USER_CTRL    =  0x6A;
const uint8_t MPU6050_REGISTER_PWR_MGMT_1   =  0x6B;
const uint8_t MPU6050_REGISTER_PWR_MGMT_2   =  0x6C;
const uint8_t MPU6050_REGISTER_CONFIG       =  0x1A;
const uint8_t MPU6050_REGISTER_GYRO_CONFIG  =  0x1B;
const uint8_t MPU6050_REGISTER_ACCEL_CONFIG =  0x1C;
const uint8_t MPU6050_REGISTER_FIFO_EN      =  0x23;
const uint8_t MPU6050_REGISTER_INT_ENABLE   =  0x38;
const uint8_t MPU6050_REGISTER_ACCEL_XOUT_H =  0x3B;
const uint8_t MPU6050_REGISTER_SIGNAL_PATH_RESET  = 0x68;

float AccelX, AccelY, AccelZ, Temperature, GyroX, GyroY, GyroZ;
WiFiClient  client;

unsigned long previousMillis = 0;
const long interval = 1000;  // Send data every second

void setup() {
  Serial.begin(9600);
  Wire.begin(sda, scl);
  MPU6050_Init(0x00, 0x00);  // Set sensitivity (Accel +/-2g, Gyro +/-250 deg/sec)
  connectToWiFi();
  ThingSpeak.begin(client); 
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
    displayInfo();
  }
  ESP.wdtFeed();  // Watchdog reset
}

void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi");
  }
}

void displayInfo() {
  ThingSpeak.setField(1, AccelX);
  ThingSpeak.setField(2, AccelY);
  ThingSpeak.setField(3, AccelZ);
  ThingSpeak.setField(4, Temperature);
  ThingSpeak.setField(5, GyroX);
  ThingSpeak.setField(6, GyroY);
  ThingSpeak.setField(7, GyroZ);
  
  if (ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey) == 200) {
    Serial.println("Data successfully sent to ThingSpeak");
  } else {
    Serial.println("Failed to send data to ThingSpeak");
  }
  delay(1000);
}

void I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data) {
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.write(data);
  if (Wire.endTransmission() != 0) {
    Serial.println("I2C write failed!");
  }
}

bool Read_RawValue(uint8_t deviceAddress, uint8_t regAddress) {
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  if (Wire.endTransmission() != 0) {
    Serial.println("I2C read request failed!");
    return false;
  }

  Wire.requestFrom(deviceAddress, (uint8_t)14);
  if (Wire.available() < 14) {
    Serial.println("I2C read data incomplete!");
    return false;
  }

  AccelX = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelY = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelZ = (((int16_t)Wire.read()<<8) | Wire.read());
  Temperature = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroX = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroY = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroZ = (((int16_t)Wire.read()<<8) | Wire.read());

  return true;
}

void MPU6050_Init(uint8_t accelConfig, uint8_t gyroConfig) {
  delay(150);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SMPLRT_DIV, 0x07);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_1, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_2, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_CONFIG, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_GYRO_CONFIG, gyroConfig);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_CONFIG, accelConfig);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_FIFO_EN, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_INT_ENABLE, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SIGNAL_PATH_RESET, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_USER_CTRL, 0x00);
}
