#include <TinyGPS++.h>
//#include <SoftwareSerial.h>
//#include <AltSoftSerial.h>

//#define SIM_RX_PIN 17
//#define SIM_TX_PIN 16
//#define GPS_RX_PIN 8
//#define GPS_TX_PIN 9

//SoftwareSerial simSerial(SIM_RX_PIN, SIM_TX_PIN);
//AltSoftSerial Serial2(GPS_RX_PIN, GPS_TX_PIN);

//Added to git

TinyGPSPlus gps;
unsigned long currentMillis;
unsigned long previousMillis = 0;
long interval = 15000;

const char* apiKey = "FZ4KI6X4RE9P41QN"; // ThingSpeak API Key

void setup() {

  delay(2000);
  Serial.begin(115200); // console
  Serial1.begin(115200);//gsm data
  Serial2.begin(9600);//gps data
  //Serial1.begin(115200); 
  //Serial2.begin(9600);

  delay(1000);

  Serial.println("Initializing...");
  sendATcommand("AT", "OK", 2000);
  Serial.println("Setup Complete");
}

void loop() {
  
  /*
  while (Serial2.available()) {
    Serial.println(Serial2.readString());
  }
  */

  //while (Serial.available()) {
  //  Serial1.println(Serial.readString());
  //}


  //while (Serial2.available()) {
  //  gps.encode(Serial2.read());    
  //}

  //Serial.println("A");
  //Serial2.println("A");
  //delay(1000);

  //currentMillis = millis();
  //if (currentMillis - previousMillis > interval) {
  //  previousMillis = currentMillis;
  //  //sendGpsToThingSpeak();
  //  Serial.println("Timeout");
  //  Serial.println(gps.location.lat(), 6);
  //}

}

void sendGpsToThingSpeak() {
  while (Serial2.available() > 0) {
    if (gps.encode(Serial2.read())) {
      float latitude = gps.location.lat();
      float longitude = gps.location.lng();

      Serial.print("Latitude: ");
      Serial.println(latitude, 6);
      Serial.print("Longitude: ");
      Serial.println(longitude, 6);

      String url = "GET https://api.thingspeak.com/update?api_key=";
      url += apiKey;
      url += "&field1=";
      url += String(latitude, 6);
      url += "&field2=";
      url += String(longitude, 6);

      sendATcommand("AT+HTTPINIT", "OK", 2000);
      sendATcommand("AT+HTTPPARA=\"URL\",\"" + url + "\"", "OK", 1000);
      sendATcommand("AT+HTTPACTION=0", "200", 10000);
      sendATcommand("AT+HTTPREAD", "+HTTPREAD:", 10000); 
      sendATcommand("AT+HTTPTERM", "OK", 2000);
      Serial.println("Data sent to ThingSpeak.");
    }
  }
}

int8_t sendATcommand(String ATcommand, String expected_answer, unsigned int timeout) {
  String response = "";
  Serial.print("Command: ");
  Serial.println(ATcommand);
  Serial1.println(ATcommand);
  unsigned long previous = millis();
  while (millis() - previous < timeout) {
    if (Serial1.available() > 0) {
      char c = Serial1.read();
      response += c;
      if (response.indexOf(expected_answer) != -1) {
        Serial.print("Response: ");
        Serial.println(response);
        return 1;
      }
    }
  }
  Serial.print("Response: ");
  Serial.println(response);
  return 0;
}
