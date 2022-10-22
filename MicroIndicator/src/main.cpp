#include <config.h>

#include <Arduino.h>

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

#define APIRequestInterval 2000 // 10 seconds

#define ApiErrorColour 0, 0, 255 // Blue

#ifdef OUTPUT_NEOPIXEL
#include <Adafruit_NeoPixel.h>

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel pixels(NeoPixelCount, NeoPixelPin, NEO_GRB + NEO_KHZ400);
#endif

#ifdef OUTPUT_TWATCH
#include <LilyGoWatch.h>
#include <TTGO.h>
TTGOClass *ttgo;
extern lv_font_t jetbrains_mono_64;
#endif

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
  const char *timestamp;
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

  bool connectionSuccess = http.begin(client, url);
  if (!connectionSuccess)
  {
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

#ifdef OUTPUT_TWATCH
void ttgoPoweroff() {
  ttgo->shutdown();
}
#endif

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

  if (currentPowerStats.coal.generation_mw > 0 || currentPowerStats.diesel.generation_mw > 0)
  {
    double halfMaxCoal = round(currentPowerStats.coal.capacity_mw / 2 + currentPowerStats.diesel.capacity_mw / 2);
    double badGeneration = (currentPowerStats.coal.generation_mw + currentPowerStats.diesel.generation_mw) / halfMaxCoal;
    if (badGeneration > 1)
    {
      badGeneration = 1;
    }

    output.color[0] = 150 + round(105 * badGeneration);
    output.color[1] = round(95 - (95 * badGeneration));
    output.powerSocketEnabled = false;
  }
  else
  {
    double halfMaxGas = round(currentPowerStats.gas.capacity_mw / 2);
    double mediumGeneration = currentPowerStats.gas.generation_mw / halfMaxGas;
    if (mediumGeneration > 1)
    {
      mediumGeneration = 1;
    }

    output.color[0] = round(160 * mediumGeneration);
    output.color[1] = round(255 - (127 * mediumGeneration));
    output.powerSocketEnabled = true;
  }

  return output;
}

void printStats(InstructionPoint instructionPoint)
{
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

#ifdef OUTPUT_NEOPIXEL
void updateNeoPixels(InstructionPoint instructionPoint)
{
  pixels.fill(pixels.Color(instructionPoint.color[0], instructionPoint.color[1], instructionPoint.color[2]), 0, NeoPixelCount);
  pixels.show();
}
#endif

#ifdef OUTPUT_TWATCH
void TWatchUpdateMeta() {
    lv_style_t metaTextStyle;
    lv_style_init(&metaTextStyle);
    lv_style_set_text_font(&metaTextStyle, LV_STATE_DEFAULT, &lv_font_montserrat_14);

    // Charging Indicator
    lv_obj_t *chargingIndicator = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(chargingIndicator, LV_SYMBOL_CHARGE);
    lv_obj_align(chargingIndicator, NULL, LV_ALIGN_IN_TOP_LEFT, 16, 16);


    char* batteryVoltageFormatted;
    if(0 > asprintf(&batteryVoltageFormatted, "%.2fv", (ttgo->power->getBattVoltage()/1000.0))) return;

    lv_obj_t *batteryVoltageLabel = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(batteryVoltageLabel, batteryVoltageFormatted);
    lv_obj_add_style(batteryVoltageLabel, LV_LABEL_PART_MAIN, &metaTextStyle);
    lv_obj_align(batteryVoltageLabel, NULL, LV_ALIGN_IN_TOP_MID, 0, 16);

    char* percentBatteryFormatted;
    if(0 > asprintf(&percentBatteryFormatted, "%d%%", ttgo->power->getBattPercentage())) return;

    lv_obj_t *batteryPercentLabel = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(batteryPercentLabel, percentBatteryFormatted);
    lv_obj_add_style(batteryPercentLabel, LV_LABEL_PART_MAIN, &metaTextStyle);
    lv_obj_align(batteryPercentLabel, NULL, LV_ALIGN_IN_TOP_RIGHT, -16, 16);

    lv_obj_t *orgLabel = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(orgLabel, "DirtyWatts - QRL");
    lv_obj_add_style(orgLabel, LV_LABEL_PART_MAIN, &metaTextStyle);
    lv_obj_align(orgLabel, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -8);
}
#endif

void setup()
{
  // put your setup code here, to run once:
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Booting");

#ifdef OUTPUT_NEOPIXEL
  pixels.begin(); // INITIALIZE NeoPixel strip object
  delay(500);
  pixels.clear(); // Set all pixel colors to 'off'
  pixels.show();
  pixels.rainbow(0, 5);
  pixels.show();
#endif

#ifdef OUTPUT_TWATCH
Serial.println("Setting up TTGO");
  ttgo = TTGOClass::getWatch();
  ttgo->begin();
  ttgo->openBL();
  ttgo->lvgl_begin();

  ttgo->powerAttachInterrupt(ttgoPoweroff);

  lv_obj_t *text = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(text, "Connecting to wifi...");
  lv_obj_align(text, NULL, LV_ALIGN_CENTER, 0, 0);

  TWatchUpdateMeta();

  lv_task_handler();


#endif

  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  // reset saved settings
  // wifiManager.resetSettings();
  wifiManager.setConnectRetries(6);
  wifiManager.setDarkMode(true);
  wifiManager.autoConnect();
  Serial.println("Wifi manager complete");
}

void loop()
{
  bool connectionResult = updatePowerStats();
  if (connectionResult)
  {
    InstructionPoint instructionPoint = calculateLightRGB(currentPowerStats);
    printStats(instructionPoint);
#ifdef OUTPUT_NEOPIXEL
    updateNeoPixels(instructionPoint);
#endif
#ifdef OUTPUT_TWATCH
    // Clear screen
    lv_obj_clean(lv_scr_act());
    // Write percent to lvgl
    int percentRenewable = round(instructionPoint.percentRenewable * 100);
    char* percentRenewableFormatted;
    if(0 > asprintf(&percentRenewableFormatted, "%d%%", percentRenewable)) return;

    lv_style_t percentRenewableStyle;
    lv_style_init(&percentRenewableStyle);
    lv_style_set_text_font(&percentRenewableStyle, LV_STATE_DEFAULT, &jetbrains_mono_64);
    lv_obj_t *renewableLabel = lv_label_create(lv_scr_act(), NULL);
    
    lv_label_set_text(renewableLabel, percentRenewableFormatted);
    lv_obj_add_style(renewableLabel, LV_LABEL_PART_MAIN, &percentRenewableStyle);
    lv_obj_align(renewableLabel, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_style_t titleTextStyle;
    lv_style_init(&titleTextStyle);
    lv_style_set_text_font(&titleTextStyle, LV_STATE_DEFAULT, &lv_font_montserrat_16);
    lv_obj_t *titleText = lv_label_create(lv_scr_act(), NULL);
    
    lv_label_set_text(titleText, "Renewable Energy");
    lv_obj_add_style(titleText, LV_LABEL_PART_MAIN, &titleTextStyle);
    lv_obj_align(titleText, NULL, LV_ALIGN_CENTER, 0, 48);

  
    
    // Set background colour
    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_local_bg_color(screen, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(instructionPoint.color[0] << 16 | instructionPoint.color[1] << 8 | instructionPoint.color[2]));
    lv_obj_set_style_local_bg_opa(screen, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_obj_set_style_local_bg_grad_color(screen, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(instructionPoint.color[0] << 16 | instructionPoint.color[1] << 8 | instructionPoint.color[2]));
    lv_obj_set_style_local_bg_grad_dir(screen, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_obj_set_style_local_bg_main_stop(screen, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_bg_grad_stop(screen, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 255);

#endif
  }
  else
  {
    Serial.println("Failed to get power stations!!");
#ifdef OUTPUT_NEOPIXEL
    pixels.fill(pixels.Color(ApiErrorColour), 0, NeoPixelCount);
    pixels.show();
#endif
  }
  Serial.println("\n");

  #ifdef OUTPUT_TWATCH
    TWatchUpdateMeta();
    lv_task_handler();
  #endif

  for (int i = 0; i < 20; i++)
  {
    delay(APIRequestInterval / 20);
    Serial.print("#");
  }
  Serial.println("\n");
}