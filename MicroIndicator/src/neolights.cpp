#ifdef OUTPUT_NEOPIXEL
#include <Adafruit_NeoPixel.h>
#include <config.h>
#include <neolights.h>
#include <powerstations.h>

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel pixels(NeoPixelCount, NeoPixelPin, NEO_GRB + NEO_KHZ400);

void updateNeoPixels(InstructionPoint instructionPoint) {
    pixels.fill(pixels.Color(instructionPoint.color[0], instructionPoint.color[1], instructionPoint.color[2]), 0, NeoPixelCount);
    pixels.show();
}

void setupNeoPixels() {
    pixels.begin();  // INITIALIZE NeoPixel strip object
    delay(500);
    pixels.clear();  // Set all pixel colors to 'off'
    pixels.show();
    pixels.rainbow(0, 5);
    pixels.show();
}

void showNeoPixelsError() {
    pixels.fill(pixels.Color(ApiErrorColour), 0, NeoPixelCount);
	pixels.show();
}
#endif