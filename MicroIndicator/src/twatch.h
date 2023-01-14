#ifdef OUTPUT_TWATCH
#pragma once
#include <Arduino.h>

#include <config.h>

#include <powerstations.h>

#define LILYGO_WATCH_LVGL
#define LV_LOG_LEVEL LV_LOG_LEVEL_INFO

#include <LilyGoWatch.h>

extern lv_font_t jetbrains_mono_64;

#if defined( LILYGO_WATCH_2020_V1 )
    #define RES_X_MAX       240
    #define RES_Y_MAX       240
#endif




class TWatch {
    private:
        TTGOClass *ttgo;
        struct tm timeinfo;
        static char ntpServer[];
        // GMT offset in seconds:
        static long  gmtOffset_sec;
        static int   daylightOffset_sec;
    public:
        void powerOff();
        void writeScreenMetaInfo();
        void setupWatch();
        void postWifiConnect();
        void refreshStats(InstructionPoint instructionPoint);
        void apiError();
        void writeScreen();
        void clearScreen();
        void syncTime();
};

void my_log_cb(lv_log_level_t level, const char * file, uint32_t line, const char * fn_name, const char * dsc);
#endif