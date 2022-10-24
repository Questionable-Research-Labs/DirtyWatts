#pragma once
#include <powerstations.h>

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