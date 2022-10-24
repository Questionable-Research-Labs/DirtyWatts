#include <twatch.h>
#include <config.h>
#include <Arduino.h>
#include <TTGO.h>
#include <powerstations.h>

TTGOClass *ttgo;
extern lv_font_t jetbrains_mono_64;

TWatch::TWatch() {
    Serial.println("Setting up TTGO");
    ttgo = TTGOClass::getWatch();
    ttgo->begin();
    ttgo->openBL();
    ttgo->lvgl_begin();

    // ttgo->powerAttachInterrupt(powerOff);

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
    lv_style_t metaTextStyle;
    lv_style_init(&metaTextStyle);
    lv_style_set_text_font(&metaTextStyle, LV_STATE_DEFAULT, &lv_font_montserrat_14);

    // Charging Indicator
    lv_obj_t *chargingIndicator = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(chargingIndicator, LV_SYMBOL_CHARGE);
    lv_obj_align(chargingIndicator, NULL, LV_ALIGN_IN_TOP_LEFT, 16, 16);

    char *batteryVoltageFormatted;
    if (0 > asprintf(&batteryVoltageFormatted, "%.2fv", (ttgo->power->getBattVoltage() / 1000.0))) return;

    lv_obj_t *batteryVoltageLabel = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(batteryVoltageLabel, batteryVoltageFormatted);
    lv_obj_add_style(batteryVoltageLabel, LV_LABEL_PART_MAIN, &metaTextStyle);
    lv_obj_align(batteryVoltageLabel, NULL, LV_ALIGN_IN_TOP_MID, 0, 16);

    char *percentBatteryFormatted;
    if (0 > asprintf(&percentBatteryFormatted, "%d%%", ttgo->power->getBattPercentage())) return;

    lv_obj_t *batteryPercentLabel = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(batteryPercentLabel, percentBatteryFormatted);
    lv_obj_add_style(batteryPercentLabel, LV_LABEL_PART_MAIN, &metaTextStyle);
    lv_obj_align(batteryPercentLabel, NULL, LV_ALIGN_IN_TOP_RIGHT, -16, 16);

    lv_obj_t *orgLabel = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(orgLabel, "DirtyWatts - QRL");
    lv_obj_add_style(orgLabel, LV_LABEL_PART_MAIN, &metaTextStyle);
    lv_obj_align(orgLabel, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -8);
}

void TWatch::postWifiConnect() {
    lv_obj_clean(lv_scr_act());

    lv_style_t titleTextStyle;
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
	// Write percent to lvgl
	int percentRenewable = round(instructionPoint.percentRenewable * 100);
	char *percentRenewableFormatted;
	if (0 > asprintf(&percentRenewableFormatted, "%d%%", percentRenewable)) return;

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
}

void TWatch::apiError() {
    	// Clear screen
	lv_obj_clean(lv_scr_act());
	// Write percent to lvgl

	lv_style_t titleTextStyle;
	lv_style_init(&titleTextStyle);
	lv_style_set_text_font(&titleTextStyle, LV_STATE_DEFAULT, &lv_font_montserrat_16);
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