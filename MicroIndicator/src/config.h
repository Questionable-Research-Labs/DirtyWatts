#pragma once


#ifdef OUTPUT_NEOPIXEL
    // Pin that the Neo Pixels are connected to
    #define NeoPixelPin D4

    // Number of LEDs in chain
    #define NeoPixelCount 30

    // Pixel type flags, add together as needed
    //   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
    //   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
    //   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
    //   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
    //   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
    #define NeoPixelFlags NEO_GRB + NEO_KHZ800

#endif

#define APIRequestInterval 2000	 // 2 seconds, measured in milliseconds

#define ApiErrorColour 150, 150, 255  // Light Blue

#ifdef OUTPUT_RELAY
    #define RelayPin 12
#endif