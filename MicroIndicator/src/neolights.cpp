#ifdef OUTPUT_NEOPIXEL
#include <Adafruit_NeoPixel.h>
#include <config.h>
#include <neolights.h>
#include <powerstations.h>

// Configured in "config.h"
// Parameter 1 = Number of pixels in strip
// Parameter 2 = Arduino pin number
// Parameter 3 = Pixel type flags, add together as needed:
Adafruit_NeoPixel pixels(NeoPixelCount, NeoPixelPin, NeoPixelFlags);

// PowerStations powerstationData
// |-> battery (Generation stats for battery power)
//     |-> generation_mw
//     |-> capacity_mw
// |-> co_gen (Generation stats for CoGeneration)
//     |-> generation_mw
//     |-> capacity_mw
// |-> coal (Generation stats for coal power)
//     |-> generation_mw
//     |-> capacity_mw
// |-> gas (Generation stats for gas power)
//     |-> generation_mw
//     |-> capacity_mw
// |-> geothermal (Generation stats for geothermal power)
//     |-> generation_mw
//     |-> capacity_mw
// |-> hydro (Generation stats for hydopower)
//     |-> generation_mw
//     |-> capacity_mw
// |-> diesel (Generation stats for diesel aka liquid power)
//     |-> generation_mw
//     |-> capacity_mw
// |-> wind (Generation stats for wind power)
//     |-> generation_mw
//     |-> capacity_mw
// |-> instructionPoint (Default algorithm)
//     |-> color (single color summary of carbon output)
//     |-> percentRenewable (0.0->1.0 percentage of renewable power)
//     |-> powerSocketEnabled (recommendation if heavy-load draws should be enabled)


// After the new Powergrid code is downloaded, this function is called to change the NeoPixel updates
void updateNeoPixels(PowerStations powerstationData) {
    int (&defaultColor)[3] = powerstationData.instructionPoint.colorLight;
    
    pixels.fill(pixels.Color(defaultColor[0], defaultColor[1], defaultColor[2]), 0, NeoPixelCount);

    pixels.show();
}

// Runs once on startup, before attempting to connect to wifi
void setupNeoPixels() {
    Serial.println("Setting up NeoPixels");
    // INITIALIZE NeoPixel strip object
    pixels.begin();
    delay(500);
    pixels.clear();  // Set all pixel colors to 'off'
    pixels.show();
    pixels.rainbow(0, 5); // Show default rainbow to show it's working
    pixels.show();
    Serial.println("NeoPixels setup complete, showing rainbow");
}


void showNeoPixelsError() {
    pixels.fill(pixels.Color(ApiErrorColour), 0, NeoPixelCount);
	pixels.show();
}
#endif
