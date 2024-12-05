#include <powerstations.h>

// PowerStations Class
// |-> co_gen (Generation stats for CoGeneration)
//     |-> generation_mw
//     |-> capacity_mw
// |-> coal (Generation stats for coal power)
//     |-> generation_mw
//     |-> capacity_mw
// |-> gas (Generation stats for gas power)
//     |-> generation_mw
//     |-> capacity_mw
// |-> geothermal (Generation stats for geothermal power)
//     |-> generation_mw
//     |-> capacity_mw
// |-> hydro (Generation stats for hydopower)
//     |-> generation_mw
//     |-> capacity_mw
// |-> diesel (Generation stats for diesel/oil aka liquid power)
//     |-> generation_mw
//     |-> capacity_mw
// |-> wind (Generation stats for wind power)
//     |-> generation_mw
//     |-> capacity_mw
// |-> solar (Generation stats for solar power)
//     |-> generation_mw
//     |-> capacity_mw
// |-> instructionPoint (Default algorithm)
//     |-> colorLight (single color summary of carbon output, for lights)
//     |-> colorScreen (single color summary of carbon output, for screens)
//     |-> percentRenewable (0.0->1.0 percentage of renewable power)
//     |-> powerSocketEnabled (recommendation if heavy-load draws should be enabled)


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

    // Calculate the total generation
    double totalRenewable = this->geothermal.generation_mw + this->hydro.generation_mw + this->wind.generation_mw + this->solar.generation_mw;
    double totalNonRenewable = this->co_gen.generation_mw + this->coal.generation_mw + this->gas.generation_mw + this->diesel.generation_mw;

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

    DoubleRange scoring_range =  { CO2E_EMISSIONS_RANGE_MIN, CO2E_EMISSIONS_RANGE_MAX };
    double scoring_datum = this->co2e_emissions;

    // DoubleRange scoring_range =  { CO2E_INTENSITY_RANGE_MIN, CO2E_INTENSITY_RANGE_MAX };
    // double scoring_datum = this->co2e_intensity;

    double scoring_datum_norm = max(min(scoring_datum, scoring_range.max), scoring_range.min);
    double scoring_datum_percent = (scoring_datum_norm - scoring_range.min) / (scoring_range.max - scoring_range.min);
    int colour_index = round(scoring_datum_percent * (COLOUR_MAP_LENGTH - 1));

    memcpy(instructionPoint.colorLight, colourRangeLight[colour_index], sizeof(instructionPoint.colorLight));
    memcpy(instructionPoint.colorScreen, colourRangeScreen[colour_index], sizeof(instructionPoint.colorScreen));

    // Calculate power socket recommendation
    instructionPoint.powerSocketEnabled = scoring_datum_percent < 0.5;

    Serial.println("CO2e emission Intensity Total:");
    Serial.println(co2e_emissions);
    Serial.println(co2e_intensity);
    Serial.println("Scoring Algorithm percent:");
    Serial.println(scoring_datum_percent);
    Serial.println("Colour Index:");
    Serial.println(colour_index);
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
    solar.generation_mw = doc["power_types"]["solar"]["generation_mw"];
    solar.capacity_mw = doc["power_types"]["solar"]["capacity_mw"];

    co2e_intensity = doc["co2e_grams_per_kwh"];
    co2e_emissions = doc["co2e_tonnne_per_hour"];

    calculateInstructionPoint();

    return true;
}

void PowerStations::SerialLogData()
{
    Serial.println("Got power stations:");

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
    Serial.print("Solar generation: ");
    Serial.println(solar.generation_mw);

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