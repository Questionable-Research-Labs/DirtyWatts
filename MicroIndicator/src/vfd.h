#pragma once
#ifdef OUTPUT_VFD
#include <powerstations.h>
#include <Arduino.h>


void setupVFDs();
void updateVFDs(InstructionPoint instructionPoint);
void showVFDError();
void writeVFD(int address, char data, boolean displayDot);

#endif