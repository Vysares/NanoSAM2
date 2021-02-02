/* hkUtil.cpp handles NS2 payload housekeeping
 * Usage:
 *  module functionality
 *      system state data collection
 *      temperature control
 * 
 *  function definitions
 *  put function declarations in memUtil.hpp
 * 
 * Modules encompassed:
 *  
 *
 * Additional files needed for compilation:
 *  none
 */

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries

// NS2 headers
#include "../headers/hkUtil.hpp"


// safe temperature range
const int MIN_SAFE_TEMP = 50;
const int MAX_SAFE_TEMP = 0;

// pins
const int DIGITAL_THERM_PIN = 14;
const int ANALOG_THERM_PIN = 15;
const int OPTICS_THEM_PIN = 16;


/* - - - - - - Module Driver Functions - - - - - - */

/* - - - - - - handleHousekeeping - - - - - - *
 * Usage:
 *  Reads incoming data from ADC, measures against baseline,
 *  appends time and pointing data, sends to memory.
 * 
 * Inputs:
 *  none
 *  
 * Outputs:
 *  None
 */

void handleHousekeeping()
{
}
