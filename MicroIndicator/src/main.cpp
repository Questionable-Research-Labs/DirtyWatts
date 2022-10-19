#include <Arduino.h>

#include <DNSServer.h>
#ifdef ARDUINO_ESP8266_NODEMCU
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
};

struct InstructionPoint
{
  int color[3];
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

  http.begin(client, url);

  if (http.GET() == HTTP_CODE_OK)
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
    Serial.println("Got json");
    const char *timestamp = doc["timestamp"];
    Serial.println("Got timestamp");
    Serial.println(timestamp);

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
    return false;
  }
}

void setup()
{
  // put your setup code here, to run once:
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Booting");

  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  // reset saved settings
  // wifiManager.resetSettings();

  // set custom ip for portal
  // wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  // fetches ssid and pass from eeprom and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect();
  Serial.println("Wifi manager complete");
}

InstructionPoint calculateLightRGB(PowerStations currentPowerStats)
{
  InstructionPoint output;
  double totalRenewable = currentPowerStats.battery.generation_mw + currentPowerStats.geothermal.generation_mw + currentPowerStats.hydro.generation_mw + currentPowerStats.wind.generation_mw;
  double totalNonRenewable = currentPowerStats.co_gen.generation_mw + currentPowerStats.coal.generation_mw + currentPowerStats.gas.generation_mw + currentPowerStats.diesel.generation_mw;
  double totalGeneration = totalRenewable + totalNonRenewable;
  double percentageRenewable = totalRenewable / totalGeneration;

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

void loop()
{
  bool connectionResult = updatePowerStats();
  if (connectionResult)
  {
    Serial.println("Got power stations");

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

    InstructionPoint instructionPoint = calculateLightRGB(currentPowerStats);
    Serial.print("Color: ");
    Serial.print(instructionPoint.color[0]);
    Serial.print(", ");
    Serial.println(instructionPoint.color[1]);
    Serial.print("Power socket: ");
    Serial.println(instructionPoint.powerSocketEnabled);

  }
  else
  {
    Serial.println("Failed to get power stations");
  }

  delay(3000);
}