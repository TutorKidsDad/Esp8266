

/*
  Generated Sketch for FCGF Library (Google Form for all Wi-Fi Boards and Arduino MKR GSM 1400)
  Documentation here : https://www.filoconnesso.it/fcgf-it/ (italian) 
                       https://www.filoconnesso.it/fcgf-en/ (english)

  Online ToolKit :     https://www.filoconnesso.it/developers/FCGFToolKit/

  Thank you for use and share with original creator reference
  By Filo Connesso https://www.filoconnesso.it
  License : https://github.com/filoconnesso/FCGF/blob/main/LICENSE
*/

//Include library
#include <FCGF.h>

//Call Google Form class
GoogleForm gf;

//Your Wi-Fi credentials
const char* your_wifi_ssid = "swapnil2@4";
const char* your_wifi_password = "TPS@462016";

//Data for your Google Form
int num_of_inputs = 2;
String myform_privateid = "14vzNW3nECSlXMw3IGA0BOlwvvfrtt7benGkbb0FCsf0";
String myform_inputs[] = {"entry.83139747","entry.1317737190"};

void setup() {
  //Enable Serial debug for 0.0.4 version and next version
  FCGF_DEBUG = true;

  //Start hardware serial
  Serial.begin(9600); 
  
  //Start Wi-Fi and Secure client for your specific board and inizialize Google Form Lib
  gf.beginWiFi(your_wifi_ssid,your_wifi_password);
}

void loop() {   
  //Set data for your inputs
  String myform_values[] = {"yourval_0","yourval_1"};
  
  //Submit data on your Google Form gf.submit(privateid,arrayinputs,arrayvalues,numofinputs);
  gf.submit(myform_privateid,myform_inputs,myform_values,num_of_inputs, 1);
  
  //Finish code
}