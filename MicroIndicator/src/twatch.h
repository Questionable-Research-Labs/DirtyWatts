#ifdef OUTPUT_TWATCH
#pragma once
#include <Arduino.h>
#include <config.h>

#include <powerstations.h>

#define LILYGO_WATCH_LVGL
#include <LilyGoWatch.h>
#if defined( LILYGO_WATCH_2020_V1 )
    #define RES_X_MAX       240
    #define RES_Y_MAX       240
#endif


class TWatch {
    private:
        TTGOClass *ttgo;
    public:
        void powerOff();
        void writeScreenMetaInfo();
        void setupWatch();
        void postWifiConnect();
        void refreshStats(InstructionPoint instructionPoint);
        void apiError();
        void writeScreen();
        void clearScreen();
};
#endif