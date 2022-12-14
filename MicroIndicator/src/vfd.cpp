#ifdef OUTPUT_VFD
#include <config.h>
#include <vfd.h>

#include <Wire.h>

const int VFD_COUNT = 3;
const int VFD_I2C_ADDRESSES[VFD_COUNT] = {0x1, 0x2, 0x3};

const char CLEAR_DISPLAY = 0x20;

void updateVFDs(InstructionPoint instructionPoint) {
    float percentRenewable = instructionPoint.percentRenewable * 100;
    // Break float into as many digits as we have VFDs
    for (int i = 0; i < VFD_COUNT; i++) {
        int digit = (int)percentRenewable % 10;
        percentRenewable /= 10;
        boolean displayDP = (instructionPoint.percentRenewable*100) / pow(10,i) <= 1 && i != VFD_COUNT - 1;
        Serial.printf("Writing %d to VFD %d with DP %d\n", digit, i, displayDP);
        
        // Convert digit to char
        char digitChar = digit + '0';

        writeVFD(VFD_I2C_ADDRESSES[i], digitChar, displayDP);
    }
}

void writeVFD(int address, char data, boolean displayDot) {
    Wire.beginTransmission(address);
    const char dataToSend[2] = {data, static_cast<char>(displayDot ? 0x1 : 0x0)};
    Wire.write(dataToSend, 2);
    Wire.endTransmission();
}

void setupVFDs() {
    // Setup I2C VFDs
    Serial.println("Starting I2C VFDs");
    Wire.begin();
    // Wait for connection
    while (Wire.status() != 0) {
        delay(100);
    }
    // Clear Displays
    Serial.println("Initialising VFDs");
    for (int i = 0; i < VFD_COUNT; i++) {
        Serial.println(i);
        writeVFD(VFD_I2C_ADDRESSES[i], CLEAR_DISPLAY, false);
    }
    Serial.println("VFDs Initialised");
}

void showVFDError() {
    for (int i = 0; i < VFD_COUNT; i++) {
        if (i == 0) {
            writeVFD(VFD_I2C_ADDRESSES[i], *("E" + '0'), false);
        } else {
            writeVFD(VFD_I2C_ADDRESSES[i], *("r" + '0'), false);
        }
    }
}

#endif