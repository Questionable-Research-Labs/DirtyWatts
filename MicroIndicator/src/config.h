#pragma once


#ifdef OUTPUT_NEOPIXEL
    #define NeoPixelPin 2 // D4 on esp8266
    #define NeoPixelCount 30
#endif

#define APIRequestInterval 2000	 // 10 seconds

#define ApiErrorColour 150, 150, 255  // Blue
