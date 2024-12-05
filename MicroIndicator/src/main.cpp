#include <Arduino.h>
#include <config.h>

#if defined(ESP8266)
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>  //https://github.com/esp8266/Arduino
#endif

#if defined(ARDUINO_ARCH_ESP32) or defined(LT_BK72XX)
#include <HTTPClient.h>
#include <WebServer.h>
#include <WiFi.h>
#endif

#include <WiFiManager.h>  //https://github.com/tzapu/WiFiManager

#ifdef OUTPUT_NEOPIXEL
#include <neolights.h>
#endif

#ifdef OUTPUT_VFD
#include <vfd.h>
#endif

#ifdef OUTPUT_TWATCH
#include <twatch.h>
TWatch twatch;
#endif

#ifdef OUTPUT_RELAY
#include <relay.h>
#endif

#include <powerstations.h>

PowerStations currentPowerStats;

unsigned int updateFailCounter = 0;

bool updatePowerStats() {
    HTTPClient http;
    WiFiClientSecure client;
    const char *host = "api.dirtywatts.nz";
    const char *url = "https://api.dirtywatts.nz/live/power_stations";

    // Not validating server
    client.setInsecure();

    int response = client.connect(host, 443);
    // Handle errors with connecting to server
    if (client.connected() == false) {
        Serial.println("Failed to connect to server");
        Serial.println(response);
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

    #ifdef ESP8266
    Serial.setDebugOutput(true);
    #endif

#ifdef OUTPUT_NEOPIXEL
    setupNeoPixels();
#endif

#ifdef OUTPUT_RELAY
    setupRelay();
#endif

#ifdef OUTPUT_VFD
    setupVFDs();
#endif

#ifdef OUTPUT_TWATCH
    twatch.setupWatch();
#endif

    Serial.println("Starting WiFiManager");

    #ifdef OUTPUT_TWATCH
        twatch.wifiSetup();
    #else
        // WiFiManager
        // Local intialization. Once its business is done, there is no need to keep it around
        WiFiManager wifiManager;

        // reset saved settings
        // wifiManager.resetSettings();
        wifiManager.setConnectRetries(6);
        wifiManager.setDarkMode(true);
        wifiManager.setCountry("NZ");
        wifiManager.autoConnect();
    #endif

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
        updateFailCounter = 0;

        currentPowerStats.SerialLogData();

#ifdef OUTPUT_NEOPIXEL
        updateNeoPixels(currentPowerStats);
#endif
#ifdef OUTPUT_VFD
        updateVFDs(currentPowerStats.instructionPoint);
#endif

#ifdef OUTPUT_TWATCH
        twatch.refreshStats(currentPowerStats.instructionPoint);
#endif

#ifdef OUTPUT_RELAY
    updateRelay(currentPowerStats);
#endif

    } else {
        Serial.println("Failed to get power stations!!");
#ifdef OUTPUT_NEOPIXEL
        showNeoPixelsError();
#endif
#ifdef OUTPUT_VFD
        showVFDError();
#endif
#ifdef OUTPUT_TWATCH
        twatch.apiError();
#endif
        updateFailCounter++;
        if (updateFailCounter > 5) {
            Serial.println("Failed to get power stations 5 times in a row. Restarting ESP");
            #ifdef OUTPUT_TWATCH
                twatch.rebootMessage();
            #endif
            ESP.restart();
        }
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