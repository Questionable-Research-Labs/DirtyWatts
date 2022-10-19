


#include <Arduino.h>

#include <Adafruit_NeoPixel.h>

#if defined(ARDUINO_ESP8266_NODEMCU) || defined(ARDUINO_ESP8266_NODEMCU_ESP12E)
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#endif
#ifdef ARDUINO_ARCH_ESP32
#include <WebServer.h>
#include <HTTPClient.h>
#include <WiFi.h>
#endif
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>


#define NeoPixelPin 2 // D4 on esp8266
#define NeoPixelCount 30

#define APIRequestInterval 2000 // 10 seconds

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel pixels(NeoPixelCount, NeoPixelPin, NEO_GRB + NEO_KHZ400);


struct PowerStats
{
  float generation_mw;
  float capacity_mw;
};

struct PowerStations
{
  PowerStats battery;
  PowerStats co_gen;
  PowerStats coal;
  PowerStats gas;
  PowerStats geothermal;
  PowerStats hydro;
  PowerStats diesel;
  PowerStats wind;
  const char* timestamp;
};

struct InstructionPoint
{
  int color[3];
  float percentRenewable;
  bool powerSocketEnabled;
};

PowerStations currentPowerStats;

bool updatePowerStats()
{
  HTTPClient http;
  WiFiClientSecure client;
  const char *host = "api.dirtywatts.nz";
  const char *url = "https://api.dirtywatts.nz/live/power_stations";

  DynamicJsonDocument doc(2048);

  // Not validating server
  client.setInsecure();

  client.connect(host, 443);
  // Handle errors with connecting to server
  if (client.connected() == false)
  {
    Serial.println("Failed to connect to server");
    delay(1000);
    return false;
  }

  bool connectionSuccess =  http.begin(client, url);
  if (!connectionSuccess) {
    Serial.println("SSL Failed to configure?!");
    return false;
  }

  int responseCode = http.GET();

  if (responseCode == HTTP_CODE_OK)
  {
    String payload = http.getString();
    auto error = deserializeJson(doc, payload);
    if (error)
    {
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(error.c_str());
      Serial.println("Got this json from server:");
      Serial.println(payload);
      Serial.println("Buffer size:");
      Serial.println((unsigned int)sizeof(payload));
      return false;
    }
    currentPowerStats.timestamp = doc["timestamp"];

    currentPowerStats.battery.generation_mw = doc["power_types"]["battery"]["generation_mw"];
    currentPowerStats.battery.capacity_mw = doc["power_types"]["battery"]["capacity_mw"];
    currentPowerStats.co_gen.generation_mw = doc["power_types"]["co_gen"]["generation_mw"];
    currentPowerStats.co_gen.capacity_mw = doc["power_types"]["co_gen"]["capacity_mw"];
    currentPowerStats.coal.generation_mw = doc["power_types"]["coal"]["generation_mw"];
    currentPowerStats.coal.capacity_mw = doc["power_types"]["coal"]["capacity_mw"];
    currentPowerStats.gas.generation_mw = doc["power_types"]["gas"]["generation_mw"];
    currentPowerStats.gas.capacity_mw = doc["power_types"]["gas"]["capacity_mw"];
    currentPowerStats.geothermal.generation_mw = doc["power_types"]["geothermal"]["generation_mw"];
    currentPowerStats.geothermal.capacity_mw = doc["power_types"]["geothermal"]["capacity_mw"];
    currentPowerStats.hydro.generation_mw = doc["power_types"]["hydro"]["generation_mw"];
    currentPowerStats.hydro.capacity_mw = doc["power_types"]["hydro"]["capacity_mw"];
    currentPowerStats.diesel.generation_mw = doc["power_types"]["diesel"]["generation_mw"];
    currentPowerStats.diesel.capacity_mw = doc["power_types"]["diesel"]["capacity_mw"];
    currentPowerStats.wind.generation_mw = doc["power_types"]["wind"]["generation_mw"];
    currentPowerStats.wind.capacity_mw = doc["power_types"]["wind"]["capacity_mw"];
    return true;
  }
  else
  {
    Serial.print("Failed to get power stats from server. Response code: ");
    Serial.println(responseCode);
    return false;
  }
}

void setup()
{
  // put your setup code here, to run once:
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Booting");

  pixels.begin(); // INITIALIZE NeoPixel strip object
  delay(500);
  pixels.clear(); // Set all pixel colors to 'off'
  pixels.show();
  pixels.rainbow(0, 5);
  pixels.show();

  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  // reset saved settings
  // wifiManager.resetSettings();
  
  wifiManager.autoConnect();
  Serial.println("Wifi manager complete");
}

InstructionPoint calculateLightRGB(PowerStations currentPowerStats)
{
  InstructionPoint output;

  output.color[0] = 0;
  output.color[1] = 0;
  output.color[2] = 0;
  output.percentRenewable = 0;
  output.powerSocketEnabled = true;

  double totalRenewable = currentPowerStats.battery.generation_mw + currentPowerStats.geothermal.generation_mw + currentPowerStats.hydro.generation_mw + currentPowerStats.wind.generation_mw;
  double totalNonRenewable = currentPowerStats.co_gen.generation_mw + currentPowerStats.coal.generation_mw + currentPowerStats.gas.generation_mw + currentPowerStats.diesel.generation_mw;
  double totalGeneration = totalRenewable + totalNonRenewable;
  if (totalGeneration == 0)
  {
    Serial.println("No generation?!?");
    output.color[2] = 255;

    return output;
  }
  double percentageRenewable = totalRenewable / totalGeneration;
  output.percentRenewable = percentageRenewable;

  if (currentPowerStats.coal.generation_mw > 0 || currentPowerStats.diesel.generation_mw > 0) {
    double halfMaxCoal = round(currentPowerStats.coal.capacity_mw / 2 + currentPowerStats.diesel.capacity_mw / 2);
    double badGeneration = (currentPowerStats.coal.generation_mw + currentPowerStats.diesel.generation_mw) / halfMaxCoal;
    if (badGeneration > 1) {
      badGeneration = 1;
    }

    output.color[0] = 150 + round(105 * badGeneration);
    output.color[1] = round(95 - (95 * badGeneration));
    output.powerSocketEnabled = false;
  }
  else {
    double halfMaxGas = round(currentPowerStats.gas.capacity_mw / 2);
    double mediumGeneration = currentPowerStats.gas.generation_mw / halfMaxGas;
    if (mediumGeneration > 1) {
      mediumGeneration = 1;
    }

    output.color[0] = round(160 * mediumGeneration);
    output.color[1] = round(255 - (127 * mediumGeneration));
    output.powerSocketEnabled = true;
  }

  return output;
}

void printStats(InstructionPoint instructionPoint) {
    Serial.println("Got power stations:");

    Serial.print("Battery generation: ");
    Serial.println(currentPowerStats.battery.generation_mw);
    Serial.print("Co-gen generation: ");
    Serial.println(currentPowerStats.co_gen.generation_mw);
    Serial.print("Coal generation: ");
    Serial.println(currentPowerStats.coal.generation_mw);
    Serial.print("Gas generation: ");
    Serial.println(currentPowerStats.gas.generation_mw);
    Serial.print("Geothermal generation: ");
    Serial.println(currentPowerStats.geothermal.generation_mw);
    Serial.print("Hydro generation: ");
    Serial.println(currentPowerStats.hydro.generation_mw);
    Serial.print("Diesel generation: ");
    Serial.println(currentPowerStats.diesel.generation_mw);
    Serial.print("Wind generation: ");
    Serial.println(currentPowerStats.wind.generation_mw);

    Serial.println("\nCalculations:");
    Serial.print("Percent renewable: ");
    Serial.println(instructionPoint.percentRenewable);
    Serial.print("Color: ");
    Serial.print(instructionPoint.color[0]);
    Serial.print(", ");
    Serial.print(instructionPoint.color[1]);
    Serial.print(", ");
    Serial.println(instructionPoint.color[2]);
    Serial.print("Power socket: ");
    Serial.println(instructionPoint.powerSocketEnabled);

}

void updateNeoPixels(InstructionPoint instructionPoint) {
  pixels.fill(pixels.Color(instructionPoint.color[0], instructionPoint.color[1], instructionPoint.color[2]), 0, NeoPixelCount);
  pixels.show();
}

void loop()
{
  bool connectionResult = updatePowerStats();
  if (connectionResult)
  {
    InstructionPoint instructionPoint = calculateLightRGB(currentPowerStats);
    printStats(instructionPoint);
    updateNeoPixels(instructionPoint);
  }
  else
  {
    Serial.println("Failed to get power stations!!");
  }
  Serial.println("\n");
  for (int i = 0; i < 20; i++) {
    delay(APIRequestInterval / 20);
    Serial.print("#");
  }
  Serial.println("\n");
}