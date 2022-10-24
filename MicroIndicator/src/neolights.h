#pragma once
#ifdef OUTPUT_NEOPIXEL
#include <powerstations.h>

void setupNeoPixels();
void updateNeoPixels(InstructionPoint instructionPoint);
void showNeoPixelsError();

#endif