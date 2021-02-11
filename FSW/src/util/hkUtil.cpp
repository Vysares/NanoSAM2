/* hkUtil.cpp handles NS2 payload housekeeping
 * Usage:
 *  module functionality
 *      system state data collection
 *      temperature control
 * 
 * Modules encompassed:
 *  houskeeping
 *  thermal control
 *
 * Additional files needed for compilation:
 *  none
 */

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries

// NS2 headers
#include "../headers/hkUtil.hpp"



/* Module Variable Definitions */
static HkData hkData[HK_SAMPLES_TO_KEEP] = {}; // array to store housekeeping data
static int hkIndex = 0;


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

/* - - - - - - Module Helper Functions - - - - - - */

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
void setHeater()
{

}

void sampleHousekeepingData()
{
    hkData[hkIndex].tempAnalog = analogRead(PIN_ANALOG_THERM);
    hkData[hkIndex].tempDigital = analogRead(PIN_DIGITAL_THERM);
    hkData[hkIndex].tempOptics = analogRead(PIN_OPTICS_THERM);
    hkData[hkIndex].analogCurrent = VOLTAGE_RES*(float)analogRead(PIN_AREG_CURR);   // analog regulator current, Amps
    hkData[hkIndex].digitalCurrent = VOLTAGE_RES*(float)analogRead(PIN_DREG_CURR);   // digital regulator current, Amps
    hkData[hkIndex].digitalRegPG = VOLTAGE_RES*(float)analogRead(PIN_DREG_PG);    // digital regulator voltage 
    hkIndex++;
}


