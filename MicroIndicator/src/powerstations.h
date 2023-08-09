#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <config.h>

#define CO2E_INTENSITY_RANGE_MIN 24
#define CO2E_INTENSITY_RANGE_MAX 159
#define CO2E_EMISSIONS_RANGE_MIN 100
#define CO2E_EMISSIONS_RANGE_MAX 975

struct PowerStats {
    float generation_mw;
    float capacity_mw;
};

struct DoubleRange {
    double min;
    double max;
};

struct InstructionPoint {
    int color[3];
    float percentRenewable;
    bool powerSocketEnabled;
    DoubleRange co2e_intensity_range;
    DoubleRange co2e_emissions_range;
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
        double co2e_intensity;
        double co2e_emissions;
        const char *timestamp;
        bool deserializePowerStations(char * httpBody);
        void SerialLogData();
        InstructionPoint instructionPoint;
    private:
        void calculateInstructionPoint();
};