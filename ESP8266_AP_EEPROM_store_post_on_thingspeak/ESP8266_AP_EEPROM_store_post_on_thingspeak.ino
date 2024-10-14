//https://www.instructables.com/Temperature-Logger-With-Nodemcu-Esp8266-D1-Mini-to/
#include <OneWire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <DallasTemperature.h>
#include <WiFiUdp.h>

#define ONE_WIRE_BUS D4
#define EE_CLEAR_BUTTON D3
#define VALUE_SUM 0
#define VALUE_NR 1

#define EE_SETTINGS_NR 8
#define EE_SSID 0
#define EE_PASS 1
#define EE_HOSTNAME 2
#define EE_TSAPIKEY 3
#define EE_STARTFIELD 4
#define EE_STARTWAIT 5
#define EE_SAMPLING 6
#define EE_CONFIGURED 7
#define EE_END 160

#define DS18B20_CYCLE_SEC 1 // Asking interval of DS18B20s
#define NTP_CYCLE_MIN 720     // Asking interval of NTP server

const int Thermometer_count_nr = 8; // Number of DS18B20s connected, maximal number is 8!!!
const int httpPort = 80;
const char* ssid = "temperature";       // Default SSID of Access Point mode
const char* passphrase = "temperature"; // Default password of Access Point mode
const char* host_name = "temperature";  // Default hostname
const char* host = "api.thingspeak.com"; // Your domain  
const char* ntpServerName = "time.nist.gov";
const int NTP_PACKET_SIZE = 48;
const unsigned long seventyYears = 2208988800UL;

int ee_size[EE_SETTINGS_NR] = {32,64,32,16,1,3,1,1};
String ee_setup[EE_SETTINGS_NR];
byte packetBuffer[ NTP_PACKET_SIZE];
unsigned long highWord;
unsigned long lowWord;
unsigned long epoch;
unsigned int localPort = 2390;      // local port to listen for UDP packets
int c = 0;
String TS_send_string;
int op_mode;
unsigned long currentMillis, previousMillis = 0;
String S_Value[8];
int Thermometer_count, Thermometer_error, send_TS, DS18B20_cycle, NTP_cycle;
float F_Value[8][2];
unsigned long secsSince1900;

IPAddress timeServerIP;
WiFiUDP udp;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

ESP8266WebServer server(80);

String content;
int statusCode, button_time;
int cycle, TS_interval_nr, ap_cycle, sec_counter, min_counter, hr_counter, sec_min_counter;
int TS_field_start;
int TS_start_sec;
int TS_interval;
int NTP_ask_interval;

//semaphores
int requestNTP, waitforNTP, NTPmisscounter, has_NTP_time, wbtype;


DeviceAddress Thermometer[8];
// Thermometer[0] = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };
// Thermometer[1] = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };
// Thermometer[2] = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };

boolean Read_EE() // returns true if EEPROM is configured, returns false if EEPROM is empty
{
  int ee_pos = 0;
  if (EEPROM.read(EE_END-1) > 1)
  { Serial.println("EEPROM not initialized, clearing...");
    Serial.println();
    for (int i = 0; i < EE_END; ++i)
    { EEPROM.write(i, 0); }
    EEPROM.commit();
  }
  // Reading EEPROM configuration
  for (int j=0; j < (EE_SETTINGS_NR-1);j++)
  { Serial.print(String(j+1) + ".: ");
    ee_setup[j] = "";
    for (int i=0; i < ee_size[j]; i++)
    { if (EEPROM.read(ee_pos) != 0)
      { ee_setup[j] += char(EEPROM.read(ee_pos)); }
      ee_pos += 1;
    }
    Serial.println(ee_setup[j]);
  }
  ee_setup[EE_CONFIGURED] = EEPROM.read(EE_END-1);
  // Final checking of configuration
  // if Hostname is empty
  if (ee_setup[EE_HOSTNAME].length() == 0)
  { ee_setup[EE_HOSTNAME] = String(host_name); }
  // if TS field start is not OK
  if ((ee_setup[EE_STARTFIELD].toInt() < 1) || (ee_setup[EE_STARTFIELD].toInt() > 8))
  {
    ee_setup[EE_STARTFIELD] = "1";
    Serial.println("TS_field_start: " + ee_setup[EE_STARTFIELD]);
  }
  TS_field_start = ee_setup[EE_STARTFIELD].toInt();

  // if TS start sec. is not OK
  if ((ee_setup[EE_STARTWAIT].toInt() < 0) || (ee_setup[EE_STARTWAIT].toInt() > 180))
  {
    ee_setup[EE_STARTWAIT] = "0";
    Serial.println("TS_start_sec: " + ee_setup[EE_STARTWAIT]);
  }
  TS_start_sec = ee_setup[EE_STARTWAIT].toInt();
  
  // if TS send interval is not OK
  TS_interval_nr = 60;
  if ((ee_setup[EE_SAMPLING].toInt() > 0) && (ee_setup[EE_SAMPLING].toInt() < 7))
  { TS_interval_nr = ee_setup[EE_SAMPLING].toInt()*60; }
  TS_interval = 0;
  Serial.println("TS_interval_nr " + String(TS_interval_nr) + " sec.");
  if (ee_setup[EE_CONFIGURED] == "1")
  {return true;}
  return false;
}

int init_DS18B20()
{ int DS18B20_count;
  DS18B20.begin();
  Serial.print("Locating DS18B20 devices...");
  Serial.print("Found ");
  DS18B20_count = DS18B20.getDeviceCount();
  Serial.print(DS18B20_count, DEC);
  Serial.print(" of " );
  Serial.print(String(Thermometer_count_nr));
  Serial.println(" devices.");
  if (DS18B20_count != Thermometer_count_nr) // if any of the DS18B20s is missing
  {
     Thermometer_error = 1;
     Serial.print(String(Thermometer_count_nr-DS18B20_count));
     Serial.println(" thermometer missing!");
  }
  for (int i=0; i<DS18B20_count; i++)
  {
    if (!DS18B20.getAddress(Thermometer[i], i))
    {
      Serial.print("Unable to find address for Thermometer ");
      Serial.println(String(i+1));
    }
    else
    {
      Serial.print("Thermometer ");
      Serial.print(String(i+1));
      Serial.print(" address: ");
      for (uint8_t k = 0; k < 8; k++)
      {
        // zero pad the address if necessary
        if (Thermometer[i][k] < 16)
        { Serial.print("0");}
        Serial.print(Thermometer[i][k], HEX);
      }
      Serial.println();
    }
  }
  Serial.println();
  return DS18B20_count;
}

void read_DS18B20()
{ float temperature;
  char temperatureString[6];
  DS18B20.requestTemperatures();
  for (int i=0; i<Thermometer_count; i++)
    {
      temperature = DS18B20.getTempC(Thermometer[i]);
      if (temperature == 85.0 || temperature == (-127.0))
      { return; }
      else
      {
        F_Value[i][VALUE_SUM] += temperature;
        F_Value[i][VALUE_NR] += 1;
      }
      if (F_Value[i][VALUE_NR] > 0)
      {
        dtostrf(F_Value[i][VALUE_SUM] / F_Value[i][VALUE_NR], 2, 1, temperatureString);
        S_Value[i] = temperatureString;
      }
      else
      {
        S_Value[i] = "-99";
      }
    }
}

void setup() {
  // Setup Serial line
  Serial.begin(115200);
  Serial.println();
  Serial.println("Startup");
  Serial.println();
  // Setup EEPROM clear button input
  pinMode(EE_CLEAR_BUTTON, INPUT_PULLUP); // EE_CLEAR_BUTTON already has external 10k pull up resistor at D1 mini
  // Disabling all WiFi activity
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  WiFi.disconnect();
  // Setup EEPROM
  EEPROM.begin(512);
  delay(10);
  // Determining whether AP or Station mode
  if (Read_EE() == false) // AP mode
  { op_mode = 0; // AP mode
  Serial.println("EEPROM empty, starting Access Point mode for configuration"); }
  else
  { op_mode = 1; // station mode
  Serial.println("EEPROM configured, starting Station mode");}
  // initialize DS18B20 thermometers
  Thermometer_count = init_DS18B20(); 
  // Reading DS18B20 temperature values
  read_DS18B20();
  // Starting web services
  if (op_mode == 0) // AP mode
  { setupAP(); }
  else              // station mode
  { setupST(); }
  // Resetting time counters
  previousMillis = millis();
  currentMillis = previousMillis;
}

void setupAP(void)
{
  // Starting AP mode
  WiFi.mode(WIFI_AP);
  WiFi.begin(ssid, passphrase);
  WiFi.hostname(host_name);
  delay(10);
  WiFi.softAP(ssid, passphrase, 6);
  Serial.println("WiFi AP started...");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  //createWebServer(webtype);
  APwebServer();
  // Start the server
  server.begin();
  Serial.println("AP webserver started."); 
}

void APwebServer()
{
   // "/" handler service
  server.on("/", []()
  {
    IPAddress ip = WiFi.softAPIP();
    String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
    content = "<!DOCTYPE HTML>\r\n<html><head><title>ESP8266 Thingspeak logger</title><style>td {height: 50px;text-align: right;vertical-align: bottom;}</style></head>";
    content += "<body style='background-color:#E6CCB3;'><center><h1>Hello!</h1><p></p><h2>IP: ";
    content += ipStr;
    content += "<p></p></h2><p></p>";
    int n = WiFi.scanNetworks();
    if (n == 0)
    {
      content += "<table><tr><td><h3>No networks found!</h3></td></tr></table>";
    }
    else
    {
      content += "<table><tr><td><ol>";
      for (int i = 0; i < n; ++i)
      {
        // Print SSID and RSSI for each network found
        content += "<li>";
        content += WiFi.SSID(i);
        content += "   (";
        content += WiFi.RSSI(i);
        content += " dBm = ";
        if (WiFi.RSSI(i)>-50)
        {content += "100";}
        else if (WiFi.RSSI(i)<-100)
        {content += "0";}
        else
        {content += 2*(WiFi.RSSI(i)+100);}
        content += (WiFi.encryptionType(i) == ENC_TYPE_NONE)?"%),   open ":"%),   protected ";
        content += "</li>";
      }
      content += "</ol></td></tr></table>";
    }
    content += "<p></p><form method='get' action='setting'><table><tr><td><p><label><h3>SSID:</h3></label></td><td><input name='ssid' length=32></p></td></tr>";
    content += "<tr><td><p><label><h3>Password:</h3></label></td><td><input type='password' name='pass' length=64></p></td></tr>";
    content += "<tr><td><p><label><h3>Hostname:</h3></label></td><td><input name='hstname' length=32 value='" + ee_setup[EE_HOSTNAME] + "'></p></td></tr>";
    content += "<tr><td><p><label><h3>ThingSpeak API key:</h3></label></td><td><input name='apikey' length=32></p></td></tr>";
    content += "<tr><td><p><label><h3>Start field (1-8):</h3></label></td><td><input name='field_no' length=32></p></td></tr>";
    content += "<tr><td><p><label><h3>Start wait (0-180 sec):</h3></label></td><td><input name='snd_start' length=32></p></td></tr>";
    content += "<tr><td><p><label><h3>Send interval (1-6 min):</h3></label></td><td valign='bottom'><input name='intval' length=32></p></td></tr></table>";
    content += "<p><input type='submit' value='SAVE'></p></form><p></p>";
    content += "<p></p><form method='get' action='cleareeprom'><input type='submit' value='Clear EEPROM'><h3>";
    for (int i=0; i<Thermometer_count; i++)
    {
      content += String(i+TS_field_start);
      content += ". temperature: ";
      content += S_Value[i];
      content += "<p></p>";
    }
    content += "</h3></center></body></html>";
    server.send(200, "text/html", content);  
  }); // "/" handler service end
  
    server.on("/setting", []()
    { int arg_count = server.args();
      String arg_string[EE_SETTINGS_NR];
      boolean setting_error = false;
      int ee_counter=0;

      if (arg_count != (EE_SETTINGS_NR-1))
      { setting_error = true; }
      else
      {
        for (int i=0; i< arg_count; i++)
        {
          if (server.arg(i).length() == 0)
          { setting_error = true; }
        }
      }
      if (setting_error == false) // there is no error present in the settings
      {
        for (int j=0; j < arg_count; j++)
        {
          Serial.print("Wrote: ");
          for (int i = 0; i < server.arg(j).length(); i++)
          {
            EEPROM.write(i+ee_counter, server.arg(j)[i]);
            Serial.print(server.arg(j)[i]); 
          }
          ee_counter += ee_size[j];
          Serial.println();
        }
        EEPROM.write(EE_END-1,1); // Station mode
        EEPROM.commit();
        content = "Data successfully saved to the EEPROM, restart in progress.";
        statusCode = 200;
        server.send(statusCode, "application/json", content);
        ESP.restart();
      }
      else // there is error in the settings
      {
        content = "Error in the settings, nothing was written to the EEPROM!";
        statusCode = 404;
        server.send(statusCode, "application/json", content);
        Serial.println("Sending 404");
      }
    });
    server.on("/cleareeprom", []()
    {
      Serial.println("Clearing EEPROM...");
      for (int i = 0; i < EE_END; i++) { EEPROM.write(i, 0); }
      EEPROM.commit();
      Serial.println("EEPROM cleanup done!");
      content = "<!DOCTYPE HTML>\r\n<html><center>";
      content += "<p></p><h2>EEPROM empty...restarting</h2></center></html>";
      server.send(200, "text/html", content);
      ESP.restart();
    });
}

void setupST()
{
  // Starting ST mode
  WiFi.mode(WIFI_STA);
  WiFi.hostname(ee_setup[EE_HOSTNAME]);
  WiFi.begin(ee_setup[EE_SSID].c_str(), ee_setup[EE_PASS].c_str());
  Serial.println("Waiting for Wifi to connect, 10 sec timeout");  
  previousMillis = millis();
  currentMillis = previousMillis;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(10);
    currentMillis = millis();
    if (currentMillis - previousMillis > 1000)
    {
      Serial.print(".");
      previousMillis += 1000;
      c += 1;
      buttonPress();
      if (c == 10)
      {
        Serial.println();
        Serial.println("Module cannot connect to AP!");
        Serial.println("RESET!");
        ESP.restart();
      } 
    }
  }
  Serial.println();
  Serial.print("Succeeded! Connected to: ");
  Serial.println(ee_setup[EE_SSID]);
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  // Starting web server for station mode
  STwebServer();
  server.begin();
  Serial.println("ST webserver started"); 
  Serial.println();
  // Starting UDP client for NTP  
  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());
  requestNTP = 1;
}

void STwebServer()
{
  server.on("/", []()
  {
    IPAddress ip = WiFi.localIP();
    String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
    content = "<!DOCTYPE HTML>\r\n<html><center><h1>Hostname: " + ee_setup[EE_HOSTNAME] + "</h1><p></p><h2>IP: ";
    content += ipStr;
    content += "</h2><p></p><h3>";
    for (int i=0; i<Thermometer_count; i++)
    {
      content += "Thingspeak field ";
      content += String(i+TS_field_start);
      content += " value: ";
      content += S_Value[i];
      content += "<p></p>";
    }        
    content += "</h3><p></p><form method='get' action='cleareeprom'><input type='submit' value='Clear EEPROM'>";
    content += "</center></html>";
    server.send(200, "text/html", content);        
  });
  server.on("/cleareeprom", []()
  {
    Serial.println("Clearing EEPROM...");
    for (int i = 0; i < EE_END; i++) { EEPROM.write(i, 0); }
    EEPROM.commit();
    Serial.println("EEPROM cleanup done!");
    content = "<!DOCTYPE HTML>\r\n<html><center>";
    content += "<p></p><h2>EEPROM empty...restarting</h2></center></html>";
    server.send(200, "text/html", content);
    ESP.restart();
  });
}


void buttonPress(void)
{
  if (digitalRead(D3) == 0) // if button is pressed 
  {
    button_time +=1;
  }
  else // Flash button is released
  {
    if (button_time > 2) // Button press timeout
    {
      Serial.println("Clearing EEPROM...");
      for (int i = 0; i < EE_END; i++) { EEPROM.write(i, 0); }
      EEPROM.commit();
      Serial.println("EEPROM cleanup done!");
      ESP.restart();
    }
    else
    {
      button_time = 0;
    }
  }
}

unsigned long sendNTPpacket(IPAddress& address)
{
  Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  
  // Initialize values needed to form NTP request
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;            // Stratum, or type of clock
  packetBuffer[2] = 6;            // Polling Interval
  packetBuffer[3] = 0xEC;         // Peer Clock Precision

  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // Send a packet requesting a timestamp
  udp.beginPacket(address, 123);   // NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  unsigned long bytesSent = udp.endPacket();   // Get the number of bytes sent
  
  return bytesSent;   // Return the number of bytes sent
}



void loop() {
  server.handleClient();
  currentMillis = millis();
  if (currentMillis < previousMillis) // Millisec overflow
  {
    previousMillis = 4294967295UL - previousMillis;
    currentMillis += previousMillis;
    currentMillis += 1;
    previousMillis = 0;
  }

  if (currentMillis - previousMillis >= 1000) // second based cycle
  {
    previousMillis += 1000;
    
    Serial.println(String(hr_counter) + ":" + String(min_counter) + ":" + String(sec_counter) + "   TS_interval:" + String(TS_interval));
    
    if (sec_min_counter == TS_start_sec)
    {
      TS_interval = 0;
      if (has_NTP_time == 1)// the controller has exact time
      {
        send_TS = 1;
      }
      else
      {
        for (int i = 0; i< 8; i++)
        {
          F_Value[i][VALUE_SUM] = 0;
          F_Value[i][VALUE_NR] = 0;
        }
      }
    }
    if (sec_min_counter > TS_start_sec)
    {
      TS_interval += 1;
      if (TS_interval >= TS_interval_nr)
      {
        TS_interval = 0;
        if (has_NTP_time == 1)// the controller has exact time
        {
          send_TS = 1;
        }
        else
        {
          for (int i = 0; i< 8; i++)
          {
            F_Value[i][VALUE_SUM] = 0;
            F_Value[i][VALUE_NR] = 0;
          }
        }
      }
    }
    sec_counter += 1;
    sec_min_counter += 1;

    buttonPress();

    DS18B20_cycle += 1;
    if (DS18B20_cycle == DS18B20_CYCLE_SEC)
    {
      DS18B20_cycle = 0;
      read_DS18B20();
    }
    
    if (waitforNTP == 1)
    {
      int cb = udp.parsePacket();
      if (!cb)
      {
        Serial.println("No NTP received packet yet");
        NTPmisscounter += 1;
        if (NTPmisscounter == 60)
        {
          Serial.println("1 minutes NTP timeout elapsed. RESET!");
          ESP.restart();
        }
      }
      else
      {
        NTPmisscounter = 0;
        waitforNTP = 0;
        Serial.print("NTP packet received, length=");
        Serial.println(cb);
        udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
        highWord = word(packetBuffer[40], packetBuffer[41]);
        lowWord = word(packetBuffer[42], packetBuffer[43]);
        secsSince1900 = highWord << 16 | lowWord;
        epoch = secsSince1900 - seventyYears;
        Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
        hr_counter = int((epoch  % 86400L) / 3600);
        Serial.print(String(hr_counter)); // print the hour (86400 equals secs per day)
        Serial.print(':');
        min_counter = int((epoch  % 3600) / 60);
        if ( min_counter < 10 )
        {
          // In the first 10 minutes of each hour, we'll want a leading '0'
          Serial.print('0');
        }
        Serial.print(String(min_counter)); // print the minute (3600 equals secs per minute)
        Serial.print(':');
        sec_counter = int(epoch % 60);
        if ( sec_counter < 10 )
        {
          // In the first 10 seconds of each minute, we'll want a leading '0'
          Serial.print('0');
        }
        Serial.println(String(sec_counter)); // print the second
        Serial.println("");
        sec_min_counter = min_counter*60+sec_counter;
        if (sec_min_counter >= TS_start_sec)
        {
          sec_min_counter -= TS_start_sec;
          while (sec_min_counter >= TS_interval_nr)
          {
            sec_min_counter -= TS_interval_nr;
          }
          TS_interval = sec_min_counter;
        }
        sec_min_counter = min_counter*60+sec_counter;
        has_NTP_time = 1;
      }
    }

     
    if (sec_counter == 60)
    {
      sec_counter = 0;
      min_counter += 1;
      NTP_cycle += 1;
      if (NTP_cycle >= NTP_CYCLE_MIN)
      {
        NTP_cycle = 0;
        requestNTP = 1;
        has_NTP_time = 0;
      }
      if (op_mode == 0)  // Access Point mode
      {
        ap_cycle += 1;
        Serial.print("Time left to reset AP mode: ");
        Serial.print(15-ap_cycle);
        Serial.println(" min.");
        Serial.println("");
        for (int i=0; i<Thermometer_count; i++)
        {
          Serial.println(String(i+1) + ". thermometer: " + S_Value[i] + "*C");
        }
        Serial.println();
        if (ap_cycle == 15) 
        {
          Serial.println("AP timeout, restart...");
          ESP.restart();
        }
      }

      if (min_counter == 60)
      {
        min_counter = 0;
        sec_min_counter = 0;
        TS_interval = 0;
        hr_counter += 1;
        if (hr_counter == 24)
        {
          hr_counter = 0;
        }
      }
    }
  }

  if (requestNTP == 1)
  {
      WiFi.hostByName(ntpServerName, timeServerIP); 
      sendNTPpacket(timeServerIP); // send an NTP packet to a time server
      requestNTP = 0;
      waitforNTP = 1;
  }

  if (send_TS == 1) // sending datas to ThingSpeak.com
  {
    send_TS = 0;
    if (op_mode == 1) // Station mode, connects to an AP
    {
      WiFiClient client;
      if (!client.connect(host, httpPort))
         {
            Serial.println(String("Connection to ") + ee_setup[EE_SSID] + String(" failed"));
            cycle += 1;
            if (cycle == 3)
               {
                  cycle = 0;
                  Serial.println("Failed to send 3 times, RESET...");
                  ESP.restart();
                }
            return;
         }
      TS_send_string = "GET /update?api_key=";
      TS_send_string += ee_setup[EE_TSAPIKEY];
      Serial.print("Sent to Thingspeak: ");
      for (int i=0; i<Thermometer_count; i++)
      {
        TS_send_string += "&field";
        TS_send_string += String(i+TS_field_start);
        TS_send_string += "=";
        TS_send_string += S_Value[i];      
        Serial.print(S_Value[i]);
        Serial.print("     ");
        F_Value[i][VALUE_SUM] = 0;
        F_Value[i][VALUE_NR] = 0;
      }
      TS_send_string += " HTTP/1.1\r\n";
      TS_send_string += "Host: ";
      TS_send_string += host;
      TS_send_string += "\r\n";
      TS_send_string += "Connection: keep-alive\r\n\r\n";
      client.print(TS_send_string);
      Serial.println("");
    }
  }
}
