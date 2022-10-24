#include <Arduino.h>
#include <config.h>

#if defined(ARDUINO_ESP8266_NODEMCU) || defined(ARDUINO_ESP8266_NODEMCU_ESP12E)
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>  //https://github.com/esp8266/Arduino
#endif

#ifdef ARDUINO_ARCH_ESP32
#include <HTTPClient.h>
#include <WebServer.h>
#include <WiFi.h>
#endif

#include <WiFiManager.h>  //https://github.com/tzapu/WiFiManager

#ifdef OUTPUT_NEOPIXEL
#include <neolights.h>
#endif

#ifdef OUTPUT_TWATCH
#include <twatch.h>
TWatch twatch;
#endif

#include <powerstations.h>

PowerStations currentPowerStats;

bool updatePowerStats() {
    HTTPClient http;
    WiFiClientSecure client;
    const char *host = "api.dirtywatts.nz";
    const char *url = "https://api.dirtywatts.nz/live/power_stations";

    // Not validating server
    client.setInsecure();

    client.connect(host, 443);
    // Handle errors with connecting to server
    if (client.connected() == false) {
        Serial.println("Failed to connect to server");
        delay(1000);
        return false;
    }

    bool connectionSuccess = http.begin(client, url);
    if (!connectionSuccess) {
        Serial.println("SSL Failed to configure?!");
        return false;
    }

    int responseCode = http.GET();

    if (responseCode == HTTP_CODE_OK) {
        String payload = http.getString();
        return currentPowerStats.deserializePowerStations((char *)payload.c_str());
    } else {
        Serial.print("Failed to get power stats from server. Response code: ");
        Serial.println(responseCode);
        return false;
    }
}



void setup() {
    Serial.begin(115200);
    Serial.println("DirtyWatts MicroIndicator Booting");

#ifdef OUTPUT_NEOPIXEL
    setupNeoPixels();
#endif

#ifdef OUTPUT_TWATCH
    twatch = TWatch();
#endif

    // WiFiManager
    // Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;

    // reset saved settings
    // wifiManager.resetSettings();
    wifiManager.setConnectRetries(6);
    wifiManager.setDarkMode(true);
    wifiManager.autoConnect();
    Serial.println("Wifi Setup Complete");

#ifdef OUTPUT_TWATCH
    twatch.postWifiConnect();
#endif
}

void loop() {
#ifdef OUTPUT_TWATCH
    twatch.clearScreen();
#endif

    bool connectionResult = updatePowerStats();
    if (connectionResult) {
        currentPowerStats.SerialLogData();

#ifdef OUTPUT_NEOPIXEL
        updateNeoPixels(currentPowerStats.instructionPoint);
#endif

#ifdef OUTPUT_TWATCH
        twatch.refreshStats(currentPowerStats.instructionPoint);
#endif

    } else {
        Serial.println("Failed to get power stations!!");
#ifdef OUTPUT_NEOPIXEL
        showNeoPixelsError();
#endif
#ifdef OUTPUT_TWATCH
        twatch.apiError();
#endif
    }

#ifdef OUTPUT_TWATCH
    twatch.writeScreenMetaInfo();
    twatch.writeScreen();
#endif

    Serial.println("\n");
    for (int i = 0; i < 20; i++) {
        delay(APIRequestInterval / 20);
        Serial.print("#");
    }
    Serial.println("\n");
}