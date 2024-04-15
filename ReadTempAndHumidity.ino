/**
 * DHT11 Sensor Reader
 * This sketch reads temperature and humidity data from the DHT11 sensor and prints the values to the serial port.
 * It also handles potential error states that might occur during reading.
 *
 * Author: Dhruba Saha
 * Version: 2.1.0
 * License: MIT
 */

// Include the DHT11 library for interfacing with the sensor.
#include "sceret.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
#include <WiFiNINA.h>
#include <DHT11.h>
#define LED_PIN 3

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;
DHT11 dht11(2);
const int sendInterval = 15000;


unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

// Initialize our values
String myStatus = "";

int temperature = 0;
int humidity = 0;
bool tempAlert = false;

void setup() {
  Serial.begin(9600);      // Initialize serial 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  ThingSpeak.begin(client);  // Initialize ThingSpeak 
}

void loop() {

  ConnectWiFi();
  getTempHumidity();
  tempCheck();
  writeThingSpeak();
  delay(sendInterval); 
}

void tempCheck(){
  if (temperature > 30) {
    tempAlert = true;
    digitalWrite(LED_PIN, HIGH);
    Serial.println("Warning Temprature above 30c");
  }
  else {
    tempAlert = false;
    digitalWrite(LED_PIN, LOW);
  }

}

void writeThingSpeak(){
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(3, tempAlert);
  
  
  // set the status
  ThingSpeak.setStatus(myStatus); 
  
  // write to the ThingSpeak channel 
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  

}

void ConnectWiFi(){
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }
}


void getTempHumidity(){
  temperature = 0;
  humidity = 0;

  int result = dht11.readTemperatureHumidity(temperature, humidity);
      // Check the results of the readings.
    // If the reading is successful, print the temperature and humidity values.
    // If there are errors, print the appropriate error messages.
    if (result == 0) {
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.print(" °C\tHumidity: ");
        Serial.print(humidity);
        Serial.println(" %");
    } else {
        // Print error message based on the error code.
        Serial.println(DHT11::getErrorString(result));
    }
}