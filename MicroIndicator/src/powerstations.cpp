#include <Arduino.h>
#include <ArduinoJson.h>
#include <powerstations.h>

bool PowerStations::deserializePowerStations(char *httpBody) {
    DynamicJsonDocument doc(2048);
    auto error = deserializeJson(doc, httpBody);
    
    if (error) {
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
    
    calculateInstructionPoint();

    return true;
}

void PowerStations::calculateInstructionPoint() {
    instructionPoint.color[0] = 0;
    instructionPoint.color[1] = 0;
    instructionPoint.color[2] = 0;
    instructionPoint.percentRenewable = 0;
    instructionPoint.powerSocketEnabled = true;

    double totalRenewable = battery.generation_mw + geothermal.generation_mw + hydro.generation_mw + wind.generation_mw;
    double totalNonRenewable = co_gen.generation_mw + coal.generation_mw + gas.generation_mw + diesel.generation_mw;
    double totalGeneration = totalRenewable + totalNonRenewable;
    if (totalGeneration == 0) {
	Serial.println("No generation?!?");
	instructionPoint.color[2] = 255;
    }
    double percentageRenewable = totalRenewable / totalGeneration;
    instructionPoint.percentRenewable = percentageRenewable;

    if (coal.generation_mw > 0 || diesel.generation_mw > 0) {
	double halfMaxCoal = round(coal.capacity_mw / 2 + diesel.capacity_mw / 2);
	double badGeneration = (coal.generation_mw + diesel.generation_mw) / halfMaxCoal;
	if (badGeneration > 1) {
	    badGeneration = 1;
	}

	instructionPoint.color[0] = 150 + round(105 * badGeneration);
	instructionPoint.color[1] = round(95 - (95 * badGeneration));
	instructionPoint.powerSocketEnabled = false;
    } else {
	double halfMaxGas = round(gas.capacity_mw / 2);
	double mediumGeneration = gas.generation_mw / halfMaxGas;
	if (mediumGeneration > 1) {
	    mediumGeneration = 1;
	}

	instructionPoint.color[0] = round(160 * mediumGeneration);
	instructionPoint.color[1] = round(255 - (127 * mediumGeneration));
	instructionPoint.powerSocketEnabled = true;
    }
}

void PowerStations::SerialLogData() {
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

    Serial.println("\nCalculations:");
    Serial.print("Percent renewable: ");
    Serial.println(instructionPoint.percentRenewable);
    Serial.print("Color: ");
    Serial.print(instructionPoint.color[0]);
    Serial.print(", ");
    Serial.print(instructionPoint.color[1]);
    Serial.print(", ");
    Serial.println(instructionPoint.color[2]);
    Serial.print("Power socket: ");
    Serial.println(instructionPoint.powerSocketEnabled);
}