#pragma once
#ifdef OUTPUT_NEOPIXEL
#include <powerstations.h>

void setupNeoPixels();
void updateNeoPixels(PowerStations powerstationData);
void showNeoPixelsError();

#endif