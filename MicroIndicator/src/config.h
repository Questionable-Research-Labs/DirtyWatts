#pragma once

#ifdef OUTPUT_TWATCH
    #define LILYGO_WATCH_LVGL                   //To use LVGL, you need to enable the macro LVGL
    #include <LilyGoWatch.h>
    #include <board/twatch2020_v1.h>
    #if defined( LILYGO_WATCH_2020_V1 )
        #define RES_X_MAX       240
        #define RES_Y_MAX       240
    #endif
#endif

#ifdef OUTPUT_NEOPIXEL
    #define NeoPixelPin 2 // D4 on esp8266
    #define NeoPixelCount 30
#endif
