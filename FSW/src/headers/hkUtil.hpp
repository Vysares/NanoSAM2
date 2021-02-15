#ifndef HKUTIL_H
#define HKUTIL_H 

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries

// NS2 headers
#include "config.hpp"


/* - - - - - - Declarations - - - - - - */
void handleHousekeeping();
void setHeater();
void sampleHousekeepingData();
float voltageToTemp(float voltage);
void timeSortHkData();

struct HousekeepingData // stores housekeeping data
{
    float opticsTemp;      // optics bench temp, celsius
    float analogTemp;      // analog board temp, celsius
    float digitalTemp;     // digital board temp, celsius
    float analogCurrent;   // analog board current, amps
    float digitalCurrent;  // digital board current, amps
    float digitalRegPG;    // digital regulator "power good" signal, volts
};

static HousekeepingData latestHkSample;

struct TempVoltagePair { float temperature, voltage; };

// thermistor temperature (deg C) / voltage (Volts) lookup table
const TempVoltagePair thermLookup[] = { { -30.000F, 2.747F },
                                        { -25.000F, 2.580F },
                                        { -20.000F, 2.388F },
                                        { -15.000F, 2.177F },
                                        { -10.000F, 1.953F },
                                        { -5.000F, 1.726F },
                                        { 0.000F, 1.505F },
                                        { 5.000F, 1.296F },
                                        { 10.000F, 1.105F },
                                        { 15.000F, 0.935F },
                                        { 20.000F, 0.787F },
                                        { 25.000F, 0.660F },
                                        { 30.000F, 0.552F },
                                        { 35.000F, 0.462F },
                                        { 40.000F, 0.387F } };


#endif
