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

struct HkData // stores housekeeping data
{
    float tempOptics;      // optics bench temp, celsius
    float tempAnalog;      // analog board temp, celsius
    float tempDigital;     // digital board temp, celsius
    float analogCurrent;   // analog board current, amps
    float digitalCurrent;  // digital board current, amps
    float digitalRegPG;    // digital regulator "power good" signal, volts
};

struct TempVoltagePair { float temperature, voltage; };

const TempVoltagePair thermLookup[] = { { -40.000F, 2.999F },
                                        { -38.000F, 2.957F },
                                        { -36.000F, 2.911F },
                                        { -34.000F, 2.861F },
                                        { -32.000F, 2.806F },
                                        { -30.000F, 2.747F },
                                        { -28.000F, 2.683F },
                                        { -26.000F, 2.616F },
                                        { -24.000F, 2.544F },
                                        { -22.000F, 2.468F },
                                        { -20.000F, 2.388F },
                                        { -18.000F, 2.305F },
                                        { -16.000F, 2.220F },
                                        { -14.000F, 2.133F },
                                        { -12.000F, 2.043F },
                                        { -10.000F, 1.953F },
                                        { -8.000F, 1.862F },
                                        { -6.000F, 1.771F },
                                        { -4.000F, 1.681F },
                                        { -2.000F, 1.592F },
                                        { 0.000F, 1.505F },
                                        { 2.000F, 1.419F },
                                        { 4.000F, 1.336F },
                                        { 6.000F, 1.256F },
                                        { 8.000F, 1.179F },
                                        { 10.000F, 1.105F },
                                        { 12.000F, 1.035F },
                                        { 14.000F, 0.967F },
                                        { 16.000F, 0.904F },
                                        { 18.000F, 0.844F },
                                        { 20.000F, 0.787F },
                                        { 22.000F, 0.734F },
                                        { 24.000F, 0.684F },
                                        { 26.000F, 0.637F },
                                        { 28.000F, 0.593F },
                                        { 30.000F, 0.552F },
                                        { 32.000F, 0.514F },
                                        { 34.000F, 0.479F },
                                        { 36.000F, 0.446F },
                                        { 38.000F, 0.415F },
                                        { 40.000F, 0.387F } };


#endif
