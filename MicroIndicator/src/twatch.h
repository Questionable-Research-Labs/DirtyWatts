#ifdef OUTPUT_TWATCH
#pragma once
#include <powerstations.h>
#include <config.h>
#include <Arduino.h>

#include <LilyGoWatch.h>
#include <board/twatch2020_v1.h>
#if defined( LILYGO_WATCH_2020_V1 )
    #define RES_X_MAX       240
    #define RES_Y_MAX       240
#endif

#define LILYGO_WATCH_LVGL

#include <TTGO.h>

class TWatch {
    private:
        TTGOClass *ttgo;
    public:
        TWatch();
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