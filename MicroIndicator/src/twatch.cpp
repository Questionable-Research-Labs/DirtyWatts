#ifdef OUTPUT_TWATCH
#include <twatch.h>

TTGOClass *ttgo;
struct tm timeinfo;

bool buttonDown = false;

void userButtonPressed()
{
    buttonDown = true;
}

void userButtonReleased()
{
    buttonDown = false;
}


void TWatch::setupWatch() {
    Serial.println("Setting up TTGO");
    ttgo = TTGOClass::getWatch();
    ttgo->begin();
    ttgo->openBL();
    ttgo->lvgl_begin();

    // Setup default text color
    TWatch::textColor = LV_COLOR_BLACK;

    lv_obj_t *text = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(text, "Connecting to wifi...");
    lv_obj_align(text, NULL, LV_ALIGN_CENTER, 0, 0);

    writeScreenMetaInfo();

    writeScreen();
}

void TWatch::powerOff() {
    ttgo->shutdown();
}

void TWatch::writeScreenMetaInfo() {
    static lv_style_t metaTextStyle;
    lv_style_init(&metaTextStyle);
    lv_style_set_text_font(&metaTextStyle, LV_STATE_DEFAULT, &lv_font_montserrat_14);
    lv_style_set_text_color(&metaTextStyle, LV_STATE_DEFAULT, TWatch::textColor);


    static lv_style_t metaTextLargeStyle;
    lv_style_init(&metaTextLargeStyle);
    lv_style_set_text_font(&metaTextLargeStyle, LV_STATE_DEFAULT, &lv_font_montserrat_22);
    lv_style_set_text_color(&metaTextLargeStyle, LV_STATE_DEFAULT, TWatch::textColor);


    
    // Display Battery Voltage top center
    lv_obj_t *batteryVoltageLabel = lv_label_create(lv_scr_act(), NULL);
    // Check if battery is plugged in
    if (ttgo->power->getBattVoltage() / 1000.0 > 1.0) {
        char *batteryVoltageFormatted;
        if (0 > asprintf(&batteryVoltageFormatted, "%.2fv", (ttgo->power->getBattVoltage() / 1000.0))) return;
        lv_label_set_text(batteryVoltageLabel, batteryVoltageFormatted);
    } else {
        lv_label_set_text(batteryVoltageLabel, "N/A");
    }
    lv_obj_add_style(batteryVoltageLabel, LV_LABEL_PART_MAIN, &metaTextStyle);
    lv_obj_align(batteryVoltageLabel, NULL, LV_ALIGN_IN_TOP_RIGHT, -16, 16);

    // Not a spykids watch, we have space for the time
    getLocalTime(&timeinfo); // Use sync time + tz offset + ms since sync to produce updated time
    char currentTime[40];
    if (0 > strftime(currentTime, 40, "%I:%M %p", &timeinfo)) return;
    lv_obj_t *currentTimeLabel = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(currentTimeLabel, currentTime);
    lv_obj_add_style(currentTimeLabel, LV_LABEL_PART_MAIN, &metaTextLargeStyle);
    lv_obj_align(currentTimeLabel, NULL, LV_ALIGN_IN_TOP_MID, 0, 48);

    // We even have space for the date!
    char currentDate[11];
    if (0 > strftime(currentDate, 11, "%Y-%m-%d", &timeinfo)) return;
    lv_obj_t *currentDateLabel = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(currentDateLabel, currentDate);
    lv_obj_add_style(currentDateLabel, LV_LABEL_PART_MAIN, &metaTextStyle);
    lv_obj_align(currentDateLabel, NULL, LV_ALIGN_IN_TOP_MID, 0, 16);


    // Display the temperature
    lv_obj_t *temperatureLabel = lv_label_create(lv_scr_act(), NULL);
    char *temperatureFormatted;
    if (0 > asprintf(&temperatureFormatted, "%.1fC", ttgo->power->getTemp())) return;
    lv_label_set_text(temperatureLabel, temperatureFormatted);
    lv_obj_add_style(temperatureLabel, LV_LABEL_PART_MAIN, &metaTextStyle);
    lv_obj_align(temperatureLabel, NULL, LV_ALIGN_IN_TOP_LEFT, 16, 16);


    // Project and group branding
    lv_obj_t *projectLabel = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(projectLabel, "DirtyWatts.nz");
    lv_obj_add_style(projectLabel, LV_LABEL_PART_MAIN, &metaTextStyle);
    lv_obj_align(projectLabel, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -22);

    lv_obj_t *orgLabel = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(orgLabel, "Questionable Research Labs");
    lv_obj_add_style(orgLabel, LV_LABEL_PART_MAIN, &metaTextStyle);
    lv_obj_align(orgLabel, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -8);
}

void TWatch::postWifiConnect() {
    syncTime();

    // Clear the screen
    lv_obj_clean(lv_scr_act());

    static lv_style_t titleTextStyle;
    lv_style_init(&titleTextStyle);
    lv_style_set_text_font(&titleTextStyle, LV_STATE_DEFAULT, &lv_font_montserrat_16);
    lv_obj_t *titleText = lv_label_create(lv_scr_act(), NULL);

    lv_label_set_text(titleText, "Connected! Fetching data...");
    lv_obj_add_style(titleText, LV_LABEL_PART_MAIN, &titleTextStyle);
    lv_obj_align(titleText, NULL, LV_ALIGN_CENTER, 0, 48);

    writeScreenMetaInfo();
    writeScreen();
}

void TWatch::refreshStats(InstructionPoint instructionPoint) {
    // Decide font color: 
    // If the background is dark, use white text
    // If the background is light, use black text
    TWatch::textColor = (instructionPoint.color[0] + instructionPoint.color[1] + instructionPoint.color[2]) / 3 > 155 ? LV_COLOR_WHITE : LV_COLOR_WHITE;

    // Set background colour
	lv_obj_t *screen = lv_scr_act();
	lv_obj_set_style_local_bg_color(screen, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(instructionPoint.color[0] << 16 | instructionPoint.color[1] << 8 | instructionPoint.color[2]));
	lv_obj_set_style_local_bg_opa(screen, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_grad_color(screen, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(instructionPoint.color[0] << 16 | instructionPoint.color[1] << 8 | instructionPoint.color[2]));
	lv_obj_set_style_local_bg_grad_dir(screen, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_obj_set_style_local_bg_main_stop(screen, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_bg_grad_stop(screen, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 255);


	// Format percent into 00.0%
	float percentRenewable = instructionPoint.percentRenewable * 100;
	char *percentRenewableFormatted;
	if (0 > asprintf(&percentRenewableFormatted, "%.1f%%", percentRenewable)) return;

    // The percent is done in a custom Jetbrains mono font, massively, to make it as easy as possible to read
	static lv_style_t percentRenewableStyle;
	lv_style_init(&percentRenewableStyle);
	lv_style_set_text_font(&percentRenewableStyle, LV_STATE_DEFAULT, &jetbrains_mono_64);
    lv_style_set_text_color(&percentRenewableStyle, LV_STATE_DEFAULT, TWatch::textColor);
	lv_obj_t *renewableLabel = lv_label_create(lv_scr_act(), NULL);

	lv_label_set_text(renewableLabel, percentRenewableFormatted);
	lv_obj_add_style(renewableLabel, LV_LABEL_PART_MAIN, &percentRenewableStyle);
	lv_obj_align(renewableLabel, NULL, LV_ALIGN_CENTER, 0, 0);

	static lv_style_t titleTextStyle;
	lv_style_init(&titleTextStyle);
	lv_style_set_text_font(&titleTextStyle, LV_STATE_DEFAULT, &lv_font_montserrat_16);
    lv_style_set_text_color(&titleTextStyle, LV_STATE_DEFAULT, TWatch::textColor);
	lv_obj_t *titleText = lv_label_create(lv_scr_act(), NULL);

	lv_label_set_text(titleText, "Renewable Energy");
	lv_obj_add_style(titleText, LV_LABEL_PART_MAIN, &titleTextStyle);
	lv_obj_align(titleText, NULL, LV_ALIGN_CENTER, 0, 48);
}

void TWatch::apiError() {
	clearScreen();

	static lv_style_t titleTextStyle;
	lv_style_init(&titleTextStyle);
	lv_style_set_text_font(&titleTextStyle, LV_STATE_DEFAULT, &lv_font_montserrat_16);
    lv_style_set_text_color(&titleTextStyle, LV_STATE_DEFAULT, TWatch::textColor);
	lv_obj_t *titleText = lv_label_create(lv_scr_act(), NULL);

	lv_label_set_text(titleText, "API Error");
	lv_obj_add_style(titleText, LV_LABEL_PART_MAIN, &titleTextStyle);
	lv_obj_align(titleText, NULL, LV_ALIGN_CENTER, 0, 48);

	// Set background colour
	lv_obj_t *screen = lv_scr_act();
	int errorColour[3] = {ApiErrorColour};
	lv_obj_set_style_local_bg_color(screen, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(errorColour[0] << 16 | errorColour[1] << 8 | errorColour[2]));
	lv_obj_set_style_local_bg_opa(screen, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_grad_color(screen, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(errorColour[0] << 16 | errorColour[1] << 8 | errorColour[2]));
	lv_obj_set_style_local_bg_grad_dir(screen, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_obj_set_style_local_bg_main_stop(screen, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_bg_grad_stop(screen, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 255);
}

void TWatch::writeScreen() {
    lv_task_handler();
}
void TWatch::clearScreen() {
	lv_obj_clean(lv_scr_act());
}

void my_log_cb(lv_log_level_t level, const char * file, uint32_t line, const char * fn_name, const char * dsc)
{
  /*Send the logs via serial port*/
  if(level == LV_LOG_LEVEL_ERROR) Serial.print("ERROR: ");
  if(level == LV_LOG_LEVEL_WARN)  Serial.print("WARNING: ");
  if(level == LV_LOG_LEVEL_INFO)  Serial.print("INFO: ");
  if(level == LV_LOG_LEVEL_TRACE) Serial.print("TRACE: ");

  Serial.print("File: ");
  Serial.print(file);

  char line_str[8];
  sprintf(line_str,"%d", line);
  Serial.print("#");
  Serial.print(line_str);

  Serial.print(": ");
  Serial.print(fn_name);
  Serial.print(": ");
  Serial.println(dsc);
}

char TWatch::ntpServer[] = "pool.ntp.org";
long  TWatch::gmtOffset_sec = 13 * 60 * 60; // Hardlocked to NZ time
int   TWatch::daylightOffset_sec = 3600;

void TWatch::syncTime() {
    Serial.println("Syncing time...");
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time, retrying in 3 seconds");
        clearScreen();
        // Write error to screen
        static lv_style_t titleTextStyle;
        lv_style_init(&titleTextStyle);
        lv_style_set_text_font(&titleTextStyle, LV_STATE_DEFAULT, &lv_font_montserrat_16);
        lv_obj_t *titleText = lv_label_create(lv_scr_act(), NULL);

        lv_label_set_text(titleText, "Time Error");
        lv_obj_add_style(titleText, LV_LABEL_PART_MAIN, &titleTextStyle);
        lv_obj_align(titleText, NULL, LV_ALIGN_CENTER, 0, 48);
        writeScreen();

        delay(3000);
        syncTime();
    }
    
    Serial.println("Time synced!");
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

}

void TWatch::wifiSetup() {
    WiFi.mode(WIFI_STA);
    // WiFiManager
    // Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;

    // reset saved settings
    // wifiManager.resetSettings();
    wifiManager.setDarkMode(true);
    wifiManager.setCountry("NZ");
    wifiManager.setConnectRetries(10);
    wifiManager.setConfigPortalTimeout(360);

    bool connected = true;

    if (buttonDown) {
        static lv_style_t titleTextStyle;
        lv_style_init(&titleTextStyle);
        lv_style_set_text_font(&titleTextStyle, LV_STATE_DEFAULT, &lv_font_montserrat_16);
        lv_obj_t *titleText = lv_label_create(lv_scr_act(), NULL);

        lv_label_set_text(titleText, "Setting up config portal...");
        lv_obj_add_style(titleText, LV_LABEL_PART_MAIN, &titleTextStyle);
        lv_obj_align(titleText, NULL, LV_ALIGN_CENTER, 0, 48);
        writeScreen();

        connected = wifiManager.startConfigPortal();
    } else {
        connected = wifiManager.autoConnect();
    }

    if (!connected) {
        Serial.println("Config portal timedout, rebooting...");
        rebootMessage();
        ESP.restart();
    }
}

void TWatch::rebootMessage() {
    clearScreen();
    // Write error to screen
    static lv_style_t titleTextStyle;
    lv_style_init(&titleTextStyle);
    lv_style_set_text_font(&titleTextStyle, LV_STATE_DEFAULT, &lv_font_montserrat_16);
    lv_obj_t *titleText = lv_label_create(lv_scr_act(), NULL);

    lv_label_set_text(titleText, "Failed to connect to server 5 times, rebooting...");
    lv_obj_add_style(titleText, LV_LABEL_PART_MAIN, &titleTextStyle);
    lv_obj_align(titleText, NULL, LV_ALIGN_CENTER, 0, 48);
    writeScreen();

    delay(3000);
}

#endif