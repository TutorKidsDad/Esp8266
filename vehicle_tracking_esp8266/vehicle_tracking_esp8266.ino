 //https://www.hackster.io/TechnicalEngineer/esp8266-and-arduino-based-iot-vehicle-tracking-system-7dce03
//use mySerial instead of serial1
#include <SoftwareSerial.h>  // Include the SoftwareSerial library for serial communication on arbitrary pins
#include <LiquidCrystal.h>   // Include the LiquidCrystal library to control LCD displays

// Create SoftwareSerial objects for communication with Arduino and GPS
SoftwareSerial mySerial(2, 3);  // RX pin at 2, TX pin at 3 for Arduino
SoftwareSerial gps(10, 11);      // RX pin at 10, TX pin at 11 for GPS

// Initialize the LCD with specified pins
LiquidCrystal lcd(13, 12, 9, 8, 7, 6);

// Initialize variables
boolean No_IP = false;           // Flag to check if an IP address is obtained
String IP = "";                  // Variable to store the IP address
String webpage = "";             // Variable to store HTML content to send
int i = 0, k = 0;                // Loop counters
int gps_status = 0;              // Variable to check GPS status
String place = "<p>1. Name: [ENTER UR NAME] </p>";  // Placeholder for user name
String num = "<p>2. Vehicle No: IND001</p>";       // Placeholder for vehicle number
String cordinat = "<p>Coordinates:</p>";            // Header for coordinates
String latitude = "";          // Variable to store latitude
String longitude = "";         // Variable to store longitude
String gpsString = "";         // String to hold the incoming GPS data
char *test = "$GPGGA";         // String to test the GPS data format

void setup() {
  Serial1.begin(9600);             // Start serial communication with GPS module
  Serial.begin(9600);              // Start serial communication with the PC
  lcd.begin(16, 2);                // Initialize the LCD with 16 columns and 2 rows
  lcd.print("Vehicle Monitoring");  // Display a welcome message on the LCD
  lcd.setCursor(0, 1);             // Move cursor to the second line
  lcd.print("     System     ");    // Display system message
  delay(2000);                     // Wait for 2 seconds
  lcd.clear();                     // Clear the LCD screen
  lcd.print("Connecting....");     // Inform about connection status
  delay(1000);                     // Wait for 1 second

  // Connect to Wi-Fi using AT commands
  connect_wifi("AT", 1000);
  connect_wifi("AT+CWMODE=3", 1000);  // Set Wi-Fi mode to 3 (Station + SoftAP)
  connect_wifi("AT+CWQAP", 1000);      // Disconnect from any current AP
  connect_wifi("AT+RST", 5000);        // Reset the ESP8266 module
  FindIP(5000);                        // Find IP address

  if (!No_IP) {                       // Check if IP was obtained
    Serial.println("Connecting Wifi....");
    // Connect to the specified Wi-Fi network
    connect_wifi("AT+CWJAP=\"[ENTER UR SSID NAME] \",\"[ENTER THE PASSWORD]\"", 7000);
  }

  Serial.println("Wifi Connected");   // Confirm Wi-Fi connection
  lcd.clear();                        // Clear the LCD screen
  lcd.print("WIFI Connected");        // Display connection status
  delay(2000);                        // Wait for 2 seconds
  lcd.clear();                        // Clear the LCD
  lcd.setCursor(5, 0);               // Set cursor for IP address
  lcd.print("IP");
  lcd.setCursor(3, 1);
  lcd.print("ADDRESS");
  ip_add();                          // Display IP address on the LCD
  delay(2000);                       // Wait for 2 seconds

  connect_wifi("AT+CIPMUX=1", 100);  // Enable multiple connections
  connect_wifi("AT+CIPSERVER=1,80", 100);  // Start a TCP server on port 80
  Serial1.end();                    // End GPS serial communication
  lcd.clear();                      // Clear the LCD
  lcd.print("   GPS DATA");         // Display GPS data status
  delay(2000);                      // Wait for 2 seconds

  gps.begin(9600);                  // Start GPS communication
  get_gps();                        // Get GPS data
  coordinate_display();             // Display coordinates on LCD
  gps.end();                       // End GPS communication
  Serial1.begin(9600);              // Restart serial communication for GPS
}

void loop() {
  k = 0;                            // Reset loop counter
  Serial.println("Please Refresh Ur Page");

  while (k < 1000) {               // Loop until k reaches 1000
    k++;                            // Increment counter
    while (Serial1.available()) {  // Check for data from Serial1
      if (Serial1.find("0,CONNECT")) {  // Check for connection signal
        Serial1.end();              // End current serial communication
        gps.begin(9600);           // Start GPS communication
        get_gps();                 // Get GPS data
        gps.end();                 // End GPS communication
        Serial1.begin(9600);       // Restart serial communication for GPS
        Serial1.flush();            // Clear the serial buffer
        Serial.println("Start Printing");
        Send();                    // Send data
        coordinate_display();       // Display coordinates
        Serial.println("Done Printing");
        delay(5000);               // Wait for 5 seconds
        lcd.clear();               // Clear the LCD
        lcd.print("System Ready"); // Indicate system readiness
        delay(1000);               // Wait for 1 second
        k = 1200;                  // Break the outer loop
        break;                     // Break the inner loop
      }
    }
    delay(1);                      // Short delay
  }
}

void FindIP(int t1) {
  int t2 = millis();              // Get the current time
  while (t2 + t1 > millis()) {   // Wait for t1 milliseconds
    while (Serial1.available() > 0) { // Check for available data
      if (Serial1.find("WIFI GOT IP")) { // Check for IP confirmation
        No_IP = true;             // Set No_IP flag to true
      }
    }
  }
}

void ip_add() {
  IP = "";                        // Clear IP variable
  char ch = 0;                   // Initialize character for reading
  while (1) {                    // Infinite loop until break
    Serial1.println("AT+CIFSR"); // Get IP address
    while (Serial1.available() > 0) { // Check for available data
      if (Serial1.find("STAIP,")) { // Look for the IP address
        delay(1000);              // Wait for 1 second
        Serial.print("IP Address:"); // Print IP address label
        while (Serial1.available() > 0) { // Read the IP address
          ch = Serial1.read();    // Read character
          if (ch == '+')          // Break on '+' character
            break;
          IP += ch;               // Append character to IP string
        }
      }
      if (ch == '+')              // Break on '+' character
        break;
    }
    if (ch == '+')                // Break on '+' character
      break;
    delay(1000);                  // Wait for 1 second
  }
  lcd.clear();                   // Clear the LCD
  lcd.print(IP);                 // Display IP on LCD
  lcd.setCursor(0, 1);           // Set cursor to the second line
  lcd.print("Port: 80");         // Display port number
  Serial.print(IP);              // Print IP to Serial Monitor
  Serial.print("Port:");         // Print port label
  Serial.println(80);            // Print port number
  delay(1000);                   // Wait for 1 second
}

void connect_wifi(String cmd, int t) {
  int temp = 0, i = 0;           // Initialize temporary variables
  while (1) {                    // Infinite loop until break
    Serial.println(cmd);         // Print command to Serial Monitor
    Serial1.println(cmd);        // Send command to Serial1
    while (Serial1.available() > 0) { // Check for available data
      if (Serial1.find("OK")) {  // Look for confirmation of command execution
        i = 8;                   // Set success flag
      }
    }
    delay(t);                    // Wait for specified time
    if (i > 5)                   // Check if command was successful
      break;
    i++;                         // Increment command attempt count
  }
  if (i == 8)                    // Check for success
    Serial.println("OK");        // Print success message
  else
    Serial.println("Error");     // Print error message
  delay(1000);                   // Wait for 1 second
}

void gps_Function() {
  gpsString = "";                // Clear GPS string
  while (1) {                    // Infinite loop until break
    while (gps.available() > 0) { // Check for GPS data
      char inChar = (char)gps.read(); // Read character from GPS
      gpsString += inChar;       // Append character to GPS string
      i++;                        // Increment index
      if (i < 7) {               // Check for valid GPS string length
        if (gpsString.substring(0, 7) == test) { // Check for GPGGA string
          gps_status = 1;        // Set GPS status to valid
        }
      }
      if (i > 500)               // Break after reading 500 characters
        break;
    }
    if (gps_status == 1)         // Check if valid GPS string found
      break;                     // Break if valid GPS data is ready
  }
  Serial.println(gpsString);     // Print GPS data to Serial Monitor
}

void get_gps() {
  gps_Function();                // Call function to get GPS data
  int gpsIndex = gpsString.indexOf(","); // Find first comma
  latitude = gpsString.substring(gpsIndex + 1, gpsString.indexOf(",", gpsIndex + 1)); // Extract latitude
  longitude = gpsString.substring(gpsString.indexOf(",", gpsIndex + 1) + 1, gpsString.indexOf(",", gpsString.indexOf(",", gpsIndex + 1) + 1)); // Extract longitude
}

void coordinate_display() {
  lcd.clear();                   // Clear the LCD
  lcd.setCursor(0, 0);          // Set cursor to the first line
  lcd.print("Lat: ");           // Display latitude label
  lcd.print(latitude);          // Display latitude value
  lcd.setCursor(0, 1);          // Move cursor to the second line
  lcd.print("Long: ");          // Display longitude label
  lcd.print(longitude);         // Display longitude value
}

void Send() {
  // Code to send data to a server or client (if required)
  // The actual sending functionality should be implemented here.
}
