#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <config.h>

struct PowerStats {
    float generation_mw;
    float capacity_mw;
};

struct InstructionPoint {
    int color[3];
    float percentRenewable;
    bool powerSocketEnabled;
};

class PowerStations {
    public:
        PowerStats battery;
        PowerStats co_gen;
        PowerStats coal;
        PowerStats gas;
        PowerStats geothermal;
        PowerStats hydro;
        PowerStats diesel;
        PowerStats wind;
        const char *timestamp;
        bool deserializePowerStations(char * httpBody);
        void SerialLogData();
        InstructionPoint instructionPoint;
    private:
        void calculateInstructionPoint();
};