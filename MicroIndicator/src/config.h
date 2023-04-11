#pragma once


#ifdef OUTPUT_NEOPIXEL
    #define NeoPixelPin D1 // D1 on esp8266
    #define NeoPixelCount 30
#endif

#define APIRequestInterval 2000	 // 20 seconds

#define ApiErrorColour 150, 150, 255  // Light Blue
