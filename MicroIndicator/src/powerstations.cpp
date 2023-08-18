#include <powerstations.h>

static int errorColor[3] = {ApiErrorColour};

#define COLOUR_MAP_LENGTH 20

static int colourRangeScreen[COLOUR_MAP_LENGTH][3] = {{24,219,0}, {73,216,0}, {99,212,0}, {119,208,0}, {136,204,0}, {153,200,0}, {169,195,0}, {183,190,0}, {195,185,0}, {208,179,0}, {219,174,0}, {230,167,0}, {243,159,0}, {255,150,19}, {255,139,31}, {255,126,40}, {255,112,47}, {255,94,53}, {255,71,58}, {255,28,63}};
static int colourRangeLight[COLOUR_MAP_LENGTH][3] = {{2, 181, 0}, {17, 174, 0}, {32, 168, 0}, {47, 162, 0}, {63, 155, 0}, {81, 147, 0}, {101, 139, 0}, {120, 132, 0}, {140, 124, 0}, {160, 116, 0}, {181, 107, 0}, {201, 99, 0}, {228, 89, 0}, {255, 77, 2}, {255, 65, 4}, {255, 53, 5}, {255, 41, 7}, {255, 29, 9}, {255, 16, 11}, {255, 3, 13}};

void PowerStations::calculateInstructionPoint()
{
    // Initialise the instruction point
    memset(instructionPoint.colorScreen, 0, sizeof(instructionPoint.colorScreen)); // fill the array with zeros
    memset(instructionPoint.colorLight, 0, sizeof(instructionPoint.colorLight)); // fill the array with zeros

    instructionPoint.percentRenewable = 0;
    instructionPoint.powerSocketEnabled = true;

    instructionPoint.co2e_intensity_range =  { CO2E_INTENSITY_RANGE_MIN, CO2E_INTENSITY_RANGE_MAX };
    instructionPoint.co2e_emissions_range =  { CO2E_EMISSIONS_RANGE_MIN, CO2E_EMISSIONS_RANGE_MAX };

    // Calculate the total generation
    double totalRenewable = battery.generation_mw + geothermal.generation_mw + hydro.generation_mw + wind.generation_mw;
    double totalNonRenewable = co_gen.generation_mw + coal.generation_mw + gas.generation_mw + diesel.generation_mw;
    
    double totalGeneration = totalRenewable + totalNonRenewable;


    // Check for problems in source data
    if (totalGeneration == 0)
    {
        Serial.println("No generation?!?");
        memcpy(instructionPoint.colorScreen, errorColor, sizeof(instructionPoint.colorScreen));
        memcpy(instructionPoint.colorLight, errorColor, sizeof(instructionPoint.colorLight));
        return;
    }


    double percentageRenewable = totalRenewable / totalGeneration;
    instructionPoint.percentRenewable = percentageRenewable;

    // Calculate colour using new scoring algorithm
    double co2e_emissions_norm = max(min(co2e_emissions, instructionPoint.co2e_emissions_range.max), instructionPoint.co2e_emissions_range.min);
    double co2e_emissions_percent = (co2e_emissions_norm - instructionPoint.co2e_emissions_range.min) / (instructionPoint.co2e_emissions_range.max - instructionPoint.co2e_emissions_range.min);
    int index = round(co2e_emissions_percent * (COLOUR_MAP_LENGTH - 1));

    Serial.println("CO2e emissions:");
    Serial.println(co2e_emissions);
    Serial.println("CO2e emissions norm:");
    Serial.println(co2e_emissions_norm);
    Serial.println("CO2e emissions percent:");
    Serial.println(co2e_emissions_percent);
    Serial.println("Index:");
    Serial.println(index);
    Serial.println("Range");
    Serial.println(instructionPoint.co2e_intensity_range.max);
    Serial.println(instructionPoint.co2e_intensity_range.min);

    memcpy(instructionPoint.colorLight, colourRangeLight[index], sizeof(instructionPoint.colorLight));
    memcpy(instructionPoint.colorScreen, colourRangeScreen[index], sizeof(instructionPoint.colorScreen));

    // Calculate power socket recommendation
    instructionPoint.powerSocketEnabled = co2e_emissions_percent < 0.5;
}

bool PowerStations::deserializePowerStations(char *httpBody)
{
    DynamicJsonDocument doc(2048);
    auto error = deserializeJson(doc, httpBody);

    if (error)
    {
        Serial.print(F("deserializeJson() failed with code "));
        Serial.println(error.c_str());
        Serial.println("Got this json from server:");
        Serial.println(httpBody);
        Serial.println("Buffer size:");
        Serial.println((unsigned int)sizeof(httpBody));
        return false;
    }
    timestamp = doc["timestamp"];

    battery.generation_mw = doc["power_types"]["battery"]["generation_mw"];
    battery.capacity_mw = doc["power_types"]["battery"]["capacity_mw"];
    co_gen.generation_mw = doc["power_types"]["co_gen"]["generation_mw"];
    co_gen.capacity_mw = doc["power_types"]["co_gen"]["capacity_mw"];
    coal.generation_mw = doc["power_types"]["coal"]["generation_mw"];
    coal.capacity_mw = doc["power_types"]["coal"]["capacity_mw"];
    gas.generation_mw = doc["power_types"]["gas"]["generation_mw"];
    gas.capacity_mw = doc["power_types"]["gas"]["capacity_mw"];
    geothermal.generation_mw = doc["power_types"]["geothermal"]["generation_mw"];
    geothermal.capacity_mw = doc["power_types"]["geothermal"]["capacity_mw"];
    hydro.generation_mw = doc["power_types"]["hydro"]["generation_mw"];
    hydro.capacity_mw = doc["power_types"]["hydro"]["capacity_mw"];
    diesel.generation_mw = doc["power_types"]["diesel"]["generation_mw"];
    diesel.capacity_mw = doc["power_types"]["diesel"]["capacity_mw"];
    wind.generation_mw = doc["power_types"]["wind"]["generation_mw"];
    wind.capacity_mw = doc["power_types"]["wind"]["capacity_mw"];

    co2e_intensity = doc["co2e_grams_per_kwh"];
    co2e_emissions = doc["co2e_tonnne_per_hour"];

    calculateInstructionPoint();

    return true;
}

void PowerStations::SerialLogData()
{
    Serial.println("Got power stations:");

    Serial.print("Battery generation: ");
    Serial.println(battery.generation_mw);
    Serial.print("Co-gen generation: ");
    Serial.println(co_gen.generation_mw);
    Serial.print("Coal generation: ");
    Serial.println(coal.generation_mw);
    Serial.print("Gas generation: ");
    Serial.println(gas.generation_mw);
    Serial.print("Geothermal generation: ");
    Serial.println(geothermal.generation_mw);
    Serial.print("Hydro generation: ");
    Serial.println(hydro.generation_mw);
    Serial.print("Diesel generation: ");
    Serial.println(diesel.generation_mw);
    Serial.print("Wind generation: ");
    Serial.println(wind.generation_mw);

    Serial.println("\nCO2e:");
    Serial.print("Intensity: ");
    Serial.println(co2e_intensity);
    Serial.print("Emissions: ");
    Serial.println(co2e_emissions);

    Serial.println("\nCalculations:");
    Serial.print("Percent renewable: ");
    Serial.println(instructionPoint.percentRenewable);
    Serial.print("Color Light: ");
    Serial.print(instructionPoint.colorLight[0]);
    Serial.print(", ");
    Serial.print(instructionPoint.colorLight[1]);
    Serial.print(", ");
    Serial.println(instructionPoint.colorLight[2]);
    Serial.print("Color Screen: ");
    Serial.print(instructionPoint.colorScreen[0]);
    Serial.print(", ");
    Serial.print(instructionPoint.colorScreen[1]);
    Serial.print(", ");
    Serial.println(instructionPoint.colorScreen[2]);
    Serial.print("Power socket: ");
    Serial.println(instructionPoint.powerSocketEnabled);
}