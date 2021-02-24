#ifndef TIMING_H
#define TIMING_H 
/* Function Declarations for Timing module
 * (class definitions must be included with higher 
 *   priority than config.hpp, but this file must be 
 *   included after config.hpp to use BUFFERSIZE) */

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries

// NS2 headers
#include "config.hpp"

/* - - - - - - Enums - - - - - - - */
enum Mode { // all payload science modes
    SAFE_MODE,      // KEEP FIRST IN ENUM, disable all operations

    // add new modes here
    STANDBY_MODE,   // waiting for wakeup signal from bus
    SUNSET_MODE,    // gathering data, waiting for phododiode threshold drop
    PRE_SUNRISE_MODE, // gathering data, waiting for photodiode threshold rise
    SUNRISE_MODE,   // gathering data on timer (returns to standby)

    // end of list
    // TODO: Do we actually need this check? Maybe a bit flip could lead to invalid states being returned? 
    MODE_NOT_RECOGNIZED        // KEEP AS LAST IN ENUM, used to check that a valid mode is being used
};

/* - - - - - - Declarations - - - - - - */
void updatePayloadMode(float buffer[BUFFERSIZE], int bufIdx);
float voltageRunningMean(float buffer[BUFFERSIZE], int bufIdx);
void checkSweepChange(float buffer[BUFFERSIZE], int bufIdx);
int wrapBufferIdx(int idx);
#endif