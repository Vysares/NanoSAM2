#ifndef TIMING_H
#define TIMING_H 

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries

// NS2 headers
#include "config.hpp"

/* - - - - - - Enums - - - - - - - */
enum Mode // all payload science modes
{
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

/* - - - - - - Class Definitions - - - - - -
class ScienceMode
{
    protected:
        int mode; // according to mode enum

    public:
        ScienceMode();
        int getMode();
        void setMode();
} */

/* - - - - - - Declarations - - - - - - */
int updatePayloadMode(int currentMode, float buffer[BUFFERSIZE], int bufIdx);
float smoothBuffer(float buffer[BUFFERSIZE], int bufIdx);
void checkSweepChange(float buffer[BUFFERSIZE], int bufIdx);
int wrapBufferIdx(int idx);
// TODO recoverPayloadMode();
#endif