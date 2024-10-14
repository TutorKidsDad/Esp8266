To improve the BMP388 test code, we can make the following enhancements:

1. **Add error handling for sensor initialization** – Show a clearer message and retry mechanism in case the sensor is not detected.
2. **Modularize the code** – Separate functions for sensor readings to make the code more readable.
3. **Add flexibility for sensor communication (I2C vs. SPI)** – Use preprocessor directives to allow easy switching between I2C and SPI.
4. **Improve output formatting** – Make the Serial Monitor output clearer and easier to read.
5. **Reduce code repetition** – Use a helper function to print sensor data.

### Improvements Explained:
1. **Retry Logic for Sensor Initialization:** The code now continuously retries connecting to the BMP388 if initialization fails, rather than just halting. This is helpful for situations where the sensor might not respond immediately after a power cycle.
2. **Modular Code:** Separated sensor initialization and reading/printing into their own functions (`initBMP388` and `printSensorReadings`) for better readability and reusability.
3. **Support for I2C and SPI:** Users can easily switch between I2C and SPI by uncommenting the appropriate lines. The code is flexible for both communication protocols.
4. **Formatted Output:** The sensor data is printed with two decimal places using `Serial.printf()` to make it more readable and accurate.
5. **Clean and Clear Serial Output:** Clearer messages for each sensor reading, improving the debugging and monitoring experience.

