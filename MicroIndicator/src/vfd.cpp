#ifdef OUTPUT_VFD
#include <config.h>
#include <vfd.h>

#include <Wire.h>

const int VFD_COUNT = 3;
const int VFD_I2C_ADDRESSES[VFD_COUNT] = {0x8, 0x9, 0xA};

const char CLEAR_DISPLAY = 0x20;

void updateVFDs(InstructionPoint instructionPoint) {
    float percentRenewable = instructionPoint.percentRenewable * 100;

    // Convert float to char array
    unsigned int bufferSize = VFD_COUNT+1+1+1; // 3 digits, 1 DP, 1 null terminator, 1 space for 100.0
    char floatSerialiseBuffer[bufferSize];
    dtostrf(percentRenewable, 3, 1, floatSerialiseBuffer);
    Serial.println(floatSerialiseBuffer);
    // Find DP position
    int dpPosition = -1;
    for (int i = 0; i < VFD_COUNT; i++) {
        if (floatSerialiseBuffer[i] == '.') {
            dpPosition = i-1; // VFD index
            break;
        }
    }
    Serial.println(dpPosition);


    // Now, fill digits
    char digits[VFD_COUNT];
    unsigned int vfdIndex = 0;
    for (int i = 0; i < bufferSize; i++) {
        if (floatSerialiseBuffer[i] == '.') {
            continue;
        }
        if (floatSerialiseBuffer[i] == '\0') {
            break;
        }
        if (vfdIndex >= VFD_COUNT) {
            break;
        }
        char digitChar = floatSerialiseBuffer[i];
        Serial.printf("Writing %c to VFD %d with DP %d\n", digitChar, i, vfdIndex == dpPosition);
        writeVFD(VFD_I2C_ADDRESSES[vfdIndex], digitChar, vfdIndex == dpPosition);
        vfdIndex++;
    }
}

void writeVFD(int address, char data, boolean displayDot) {
    Wire.beginTransmission(address);
    const char dataToSend[2] = {data, static_cast<char>(displayDot ? 0x1 : 0x0)};
    Serial.printf("Writing data to VFD %d: ", address);
    Serial.println(dataToSend[0],BIN);
    Serial.println(dataToSend[1],BIN);
    Wire.write(dataToSend, 2);
    Wire.endTransmission();
}

void setupVFDs() {
    // Setup I2C VFDs
    Serial.println("Starting I2C VFDs");
    Wire.begin(D2, D1);
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
        char* letter = "E";
        if (i > 0) {
            letter = "r";
        }
        writeVFD(VFD_I2C_ADDRESSES[i], *letter, false);
    }
}

#endif