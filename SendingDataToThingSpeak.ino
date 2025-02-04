#include <TinyGPS++.h>

#define SIM_SERIAL Serial1  // SIM Module (Pins 18 TX, 19 RX)
#define GPS_SERIAL Serial2  // GPS Module (Pins 16 TX, 17 RX)

TinyGPSPlus gps;
unsigned long previousMillis = 0;
const long interval = 120000;  // Send data every 60 seconds
const char* apiKey = "AYDHNWSD93VRXC1S";  // ThingSpeak API Key

void setup() {
    Serial.begin(115200);    // Serial Monitor (PC)
    SIM_SERIAL.begin(115200);  // SIM Module (GSM)
    GPS_SERIAL.begin(9600);   // GPS Module

    delay(3000);
    Serial.println("Initializing GSM Module...");

    // Check SIM module response
    if (sendATcommand("AT", "OK", 5000)) {
        Serial.println("SIM Module Ready.");
    } else {
        Serial.println("ERROR: SIM Module not responding!");
    }

    // Check network registration
    if (sendATcommand("AT+CREG?", "+CREG: 0,1", 5000) || sendATcommand("AT+CREG?", "+CREG: 0,5", 5000)) {
        Serial.println("Network Registered!");
    } else {
        Serial.println("ERROR: Not registered on network!");
    }
}

void loop() {
    // Read data from GPS module
    
    while (GPS_SERIAL.available()) {
        gps.encode(GPS_SERIAL.read());
    }

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis > interval) {
        previousMillis = currentMillis;
        sendGpsToThingSpeak();
    }
}

void sendGpsToThingSpeak() {
    if (!gps.location.isValid()) {
        Serial.println("ERROR: GPS signal not available!");
        return;
    }

    float latitude = gps.location.lat();
    float longitude = gps.location.lng();

    Serial.print("Latitude: ");
    Serial.println(latitude, 6);
    Serial.print("Longitude: ");
    Serial.println(longitude, 6);

    // Ensure GPRS is enabled
   
    // Construct URL (use HTTP instead of HTTPS)
    String url = "http://api.thingspeak.com/update?api_key=";
    url += apiKey;
    url += "&field1=" + String(latitude, 6);
    url += "&field2=" + String(longitude, 6);

    // Send HTTP request via GSM
    if (sendATcommand("AT+HTTPINIT", "OK", 2000) &&
        //sendATcommand("AT+HTTPPARA=\"CID\",1", "OK", 1000) &&
        sendATcommand("AT+HTTPPARA=\"URL\",\"" + url + "\"", "OK", 1000) &&
        sendATcommand("AT+HTTPACTION=0", "200", 20000)) {  // Increased timeout
        
        Serial.println("Data Sent to ThingSpeak!"); 
        sendATcommand("AT+HTTPREAD", "OK", 10000);  // Read server response
    } else {
        Serial.println("ERROR: Failed to send data!"); 
    }

    sendATcommand("AT+HTTPTERM", "OK", 1000);
}

bool sendATcommand(String command, String expected_response, unsigned int timeout) {
    String response = "";
    Serial.println(command);
    SIM_SERIAL.println(command);
    unsigned long start_time = millis();

    while (millis() - start_time < timeout) {
        if (SIM_SERIAL.available()) {
            char c = SIM_SERIAL.read();
            response += c;
            if (response.indexOf(expected_response) != -1) {
                Serial.println("success");
                Serial.println(response);
                Serial.flush();
                return true;
            }
        }
    }

    Serial.println("SIM Response: " + response);  // Print GSM module response for debugging
    return false;
}
