#include <TinyGPS++.h>
//#include <SoftwareSerial.h>
//#include <AltSoftSerial.h>

//#define SIM_RX_PIN 17
//#define SIM_TX_PIN 16
//#define GPS_RX_PIN 8
//#define GPS_TX_PIN 9

//SoftwareSerial simSerial(SIM_RX_PIN, SIM_TX_PIN);
//AltSoftSerial Serial2(GPS_RX_PIN, GPS_TX_PIN);

TinyGPSPlus gps;
unsigned long currentMillis;
unsigned long previousMillis = 0;
long interval = 15000;

const char* apiKey = "FZ4KI6X4RE9P41QN"; // ThingSpeak API Key

void setup() {

  // This is some commen......

  delay(2000);
  Serial.begin(115200); // console
  Serial1.begin(9600);//gsm data
  Serial2.begin(9600);//gps data
  //Serial1.begin(115200); 
  //Serial2.begin(9600);

  delay(1000);

  Serial.println("Initializing...");
  //sendATcommand("AT", "OK", 2000);
  //Serial.println("Setup Complete");
}

void loop() {
  
  // This sketch displays information every time a new sentence is correctly encoded.
  while (Serial2.available() > 0)
    if (gps.encode(Serial2.read()))
       displayInfo();

  // If 5000 milliseconds pass and there are no characters coming in
  // over the software serial port, show a "No GPS detected" error
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println("No GPS detected");
    while(true);
  }

}


void displayInfo()
{
  if (gps.location.isValid())
  {
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
    Serial.print("Altitude: ");
    Serial.println(gps.altitude.meters());
  }
  else
  {
    Serial.println("Location: Not Available");
  }
  
  Serial.print("Date: ");
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print("/");
    Serial.print(gps.date.day());
    Serial.print("/");
    Serial.println(gps.date.year());
  }
  else
  {
    Serial.println("Not Available");
  }

  Serial.print("Time: ");
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(":");
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(":");
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(".");
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.println(gps.time.centisecond());
  }
  else
  {
    Serial.println("Not Available");
  }

  Serial.println();
  Serial.println();
  delay(1000);
}