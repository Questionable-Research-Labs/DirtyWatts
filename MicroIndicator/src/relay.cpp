#ifdef OUTPUT_RELAY

#include <relay.h>

// Runs once on startup, before attempting to connect to wifi
void setupRelay() {
    Serial.println("Setting up Relay output");
    pinMode(RelayPin, OUTPUT);

}


void updateRelay(PowerStations powerstationData) {
    digitalWrite(RelayPin, powerstationData.instructionPoint.powerSocketEnabled);
}
#endif