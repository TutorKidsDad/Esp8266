/***************************************************************************
  This is a library for the BMP3XX temperature & pressure sensor.
  Designed specifically to work with the Adafruit BMP388 Breakout:
  ----> http://www.adafruit.com/products/3966
  
  These sensors use I2C or SPI to communicate, 2 or 4 pins are required to interface.
  
  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

// Complete project details: https://RandomNerdTutorials.com/esp8266-nodemcu-bmp388-arduino/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

// Uncomment this line to use I2C (default)
#define USE_I2C

// Uncomment these lines if using SPI
/*#define BMP_SCK 14
#define BMP_MISO 12
#define BMP_MOSI 13
#define BMP_CS 15*/

#define SEALEVELPRESSURE_HPA (1013.25)  // Default sea level pressure

Adafruit_BMP3XX bmp;

// Function to initialize the BMP388 sensor
void initBMP388() {
  Serial.println("Initializing BMP388 sensor...");

  bool status;
  #ifdef USE_I2C
    status = bmp.begin_I2C(); // I2C initialization
  #else
    status = bmp.begin_SPI(BMP_CS, BMP_SCK, BMP_MISO, BMP_MOSI); // SPI initialization
  #endif

  if (!status) {
    Serial.println("Could not find a valid BMP3 sensor, check wiring!");
    while (1) {
      Serial.println("Retrying to connect to BMP388...");
      delay(5000);
      #ifdef USE_I2C
        status = bmp.begin_I2C();
      #else
        status = bmp.begin_SPI(BMP_CS, BMP_SCK, BMP_MISO, BMP_MOSI);
      #endif
      if (status) break;
    }
  }

  // Set up oversampling and filter coefficients for more accurate readings
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);

  Serial.println("BMP388 Initialized Successfully!");
}

// Function to print sensor readings
void printSensorReadings() {
  if (!bmp.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }

  // Print formatted sensor data
  Serial.printf("Temperature = %.2f *C\n", bmp.temperature);
  Serial.printf("Pressure = %.2f hPa\n", bmp.pressure / 100.0);
  Serial.printf("Approx. Altitude = %.2f m\n", bmp.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Initialize BMP388 sensor
  initBMP388();
}

void loop() {
  // Get and print sensor readings every 2 seconds
  printSensorReadings();
  delay(2000);
}
