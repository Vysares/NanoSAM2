#ifndef HK_H
#define HK_H 

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries

// NS2 headers
#include "config.hpp"


/* - - - - - - Declarations - - - - - - */
void handleHousekeeping();
void setHeater();
void sampleHousekeepingData();
float voltageToBoardTemp(float voltage);
float voltageToOpticsTemp(float voltage);
void timeSortHkData();

/* - HousekeepingData -
*   Holds a single point of housekeeping data.
*   Members: opticsTemp, analogTemp, digitalTemp,
*   analogCurrent, digitalCurrent, digitalRegPG, timeMillis 
*/
struct HousekeepingData {
    float opticsTemp;      // optics bench temp, celsius
    float analogTemp;      // analog board temp, celsius
    float digitalTemp;     // digital board temp, celsius
    float analogCurrent;   // analog board current, amps
    float digitalCurrent;  // digital board current, amps
    float digitalRegPG;    // digital regulator "power good" signal, volts
    float timeMillis;      // time that sample was taken, milliseconds
};

static HousekeepingData latestHkSample;                 // latest point of housekeeping data
struct TempVoltagePair { float temperature, voltage; }; // used for entries in thermistor lookup table

// Thermistor lookup table for board thermistors, temperature (deg C) / voltage (Volts).
const TempVoltagePair thermLookup[] = { { -40.000F, 2.999F },
                                        { -30.000F, 2.747F },
                                        { -20.000F, 2.388F },
                                        { -10.000F, 1.953F },
                                        { 0.000F, 1.505F },
                                        { 10.000F, 1.105F },
                                        { 20.000F, 0.787F },
                                        { 30.000F, 0.552F },
                                        { 40.000F, 0.387F },
                                        { 50.000F, 0.272F },
                                        { 60.000F, 0.194F },
                                        { 70.000F, 0.140F },
                                        { 80.000F, 0.102F },
                                        { 90.000F, 0.076F } };
// End thermistor lookup table

#endif
