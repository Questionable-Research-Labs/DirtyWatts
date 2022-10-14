#include <Arduino.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>


HTTPClient http;
WiFiClientSecure client;
const char* host = "https://api.dirtywatts.nz/live/power_stations";

DynamicJsonDocument doc(2048);

char requestBodyBuffer[2048];

void setup() {
  // put your setup code here, to run once:
      // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println("Booting");

    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    //reset saved settings
    //wifiManager.resetSettings();
    
    //set custom ip for portal
    //wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    wifiManager.autoConnect("AutoConnectAP");
    Serial.println("Wifi manager complete");

    client.connect(host, 433);

}

void loop() {
  // Not validating server
  client.setInsecure();

  http.begin(client, host);

  if (http.GET() == HTTP_CODE_OK) {
      // payload = http.getString();
      WiFiClient jsonStream = http.getStream();
      jsonStream.readBytesUntil('\0', requestBodyBuffer, (unsigned int)sizeof(requestBodyBuffer));
      deserializeJson(doc, requestBodyBuffer);
      Serial.println("Got json");
      const char* timestamp = doc["timestamp"]; 
      Serial.println("Got timestamp");
      Serial.println(timestamp);

  }


  delay(3000);
}