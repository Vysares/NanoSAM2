/* timing.cpp handles the state of the payload based on timing/sensor inputs
 * Usage:
 *  module functionality
 *  function definitions
 *  put function declarations in timing.hpp
 *  put constants/configuration declarations in config.hpp
 * 
 * Modules encompassed:
 *  Science window timing
 *
 * Additional files needed for compilation:
 *  config.hpp
 *  timing.hpp
 *  eventUtil.cpp & eventUtil.hpp
 *  comUtil.cpp & comUtil.hpp
 */

/* - - - - - - Includes - - - - - - */
// All libraries are put in timing.hpp
// NS2 headers
#include "../headers/timing.hpp"
#include "../headers/eventUtil.hpp"
#include "../headers/commandHandling.hpp"
#include "../headers/dataCollection.hpp"

/* Module Variable Definitions */

/* - - - - - - Class Definitions - - - - - - */
/* - - - - - - ScienceMode - - - - - - *
 * Usage:
 *  tracks the Science Mode of the payload
 */
ScienceMode::ScienceMode() { // constructor
    mode = STANDBY_MODE;
    adcsPointingAtSun = false;
}

int ScienceMode::getMode() {
    return mode;
}

void ScienceMode::setMode(int newMode) {
    if (newMode < SAFE_MODE || newMode >= MODE_NOT_RECOGNIZED) { // check if new mode is valid
        Serial.println("Attempted to set mode to unrecognized mode (Timing Module) - Defaulting to Safe Mode");
        scienceMode.setMode(SAFE_MODE);
        return;
    } else if (newMode == mode) { return; };

    if (newMode == STANDBY_MODE) {
        onStandbyEntry.invoke();
    }
    if (mode == SUNRISE_MODE || mode == SUNSET_MODE) {
        saveBufferEvent.invoke();
    }
    mode = newMode;

    Serial.print("Mode Changed: ");
    Serial.println(newMode);
}

bool ScienceMode::getPointingAtSun() {
    return adcsPointingAtSun;
}

void ScienceMode::setPointingAtSun(bool newState) {
    /*FUTURE TEAMS: Flesh out the below if statement with the logic for 
    *   controlling the payload mode based on ADCS outputs 
    * For ground testing this is hardcoded as true in config.hpp,
    *   but once ADCS is implemented you will need to dynamically update this,
    *   i.e.    ADCS_READY = (digitalRead(BUS_READY_PIN) == HIGH); */
    adcsPointingAtSun = newState;
}

void ScienceMode::sweepChange() {
    // method to check if the ADCS system has permission to change sweep direction
    // and invoke the sweep change event if so
    if (!sweepChangeLockout.checkInvoked()) {
        sweepChangeEvent.invoke();
        sweepChangeLockout.start();
    }
}

/* - - - - - - Helper Functions - - - - - - */

/* - - - - - - updatePayloadMode - - - - - - *
 * Usage:
 *  returns the new mode that the payload should be in given the current mode and the buffer 
 *  invokes/starts any events that are used in transitioning payload mode 
 *  acts on scienceMode declared in config.hpp
 * 
 * Inputs:
 *  buffer - buffer array containing most recent photodiode voltage measurements
 *  bufIdx - index of most recent measurement updated in bufIdx
 * 
 * Outputs:
 *  None
 */
void updatePayloadMode(uint16_t buffer[], int bufIdx) {
    int currentMode = scienceMode.getMode();
    
    if (currentMode == SUNSET_MODE || currentMode == PRE_SUNRISE_MODE || currentMode == SUNRISE_MODE) {
        if (scienceMode.getPointingAtSun()) {
            // smooth data to avoid a single noisy value prematurely ending a window
            float pdVoltageSmooth = voltageRunningMean(buffer, bufIdx); //photodiode voltage
            
            switch (currentMode) {
                case SUNSET_MODE: // gathering data until sun passes behind horizon
                    if (pdVoltageSmooth < SUN_THRESH_VOLTAGE) { // if the sun is not found
                        // check if it is time to change sweep direction
                        checkSweepChange(buffer, bufIdx);

                        if (sweepTimeoutEvent.checkInvoked()) {
                            // if we have waited long enough for ADCS sweeping
                            scienceMode.setMode(PRE_SUNRISE_MODE);
                        }
                    } else { 
                        // sun still found, return same mode and refresh sweep timeout
                        sweepTimeoutEvent.start();
                    }
                    break;
                
                case PRE_SUNRISE_MODE: // waiting for sun to rise above horizon
                    if (pdVoltageSmooth >= SUN_THRESH_VOLTAGE) {
                        // start sunrise event 
                        sunriseTimerEvent.start();
                        scienceMode.setMode(SUNRISE_MODE);
                    }
                    break;

                case SUNRISE_MODE: // gathering data for length of buffer
                    // check if it is time to change sweep direction
                    checkSweepChange(buffer, bufIdx);

                    // wait until sunrise data window is complete, save buffer,
                    // and return to standby
                    if (sunriseTimerEvent.checkInvoked()) {
                        scienceMode.setMode(STANDBY_MODE);
                    }
                    break;
            
                default: // mode not recognized, this should not happen and is logically impossible
                    scienceMode.setMode(MODE_NOT_RECOGNIZED);
                    break;
            }

        } else { // standby until ADCS points payload at the sun
            Serial.println("ADCS Not Ready for Science Measurements (Timing Module)");
            scienceMode.setMode(STANDBY_MODE);
        }
    }
}

/* - - - - - - voltageRunningMean - - - - - - *
 * Usage:
 *  Apply basic smoothing to a configurable number of the most recent buffer entries
 *  Avoids a single noisy measurement prematurely ending a window
 *  To change the number of indices used in smoothing, change SMOOTH_IDX_COUNT in config.hpp 
 * 
 * Inputs:
 *  buffer - array of photodiode voltages
 *  bufIdx - index of most recent photodiode measurement
 * 
 * Outputs:
 *  smoothVoltage - average of the previous SMOOTH_IDX_COUNT indices of buffer
 */
float voltageRunningMean(uint16_t buffer[], int bufIdx) {
    float smoothVoltage = 0;
    int i, idx;
    for (i = bufIdx; i > bufIdx - SMOOTH_IDX_COUNT; i--) { // decrement to move backwards in time
        
        idx = i; // I am scared to toss the iterator for this loop through wrapBufferIdx
        
        // ensure that index is within array bounds 
        // (i.e. starting with i=0 would throw error if we didn't wrap the index)
        idx = wrapBufferIdx(idx); 

        smoothVoltage += static_cast<float>(buffer[idx]) * ADC_VOLTAGE_RES; // sum voltages
    }
    return smoothVoltage / (float)SMOOTH_IDX_COUNT; // return mean
}

/* - - - - - - wrapBufferIdx - - - - - - *
 * Usage:
 *  Avoid indexing the photodiode voltage buffer with a negative value
 * 
 * Inputs:
 *  idx - an index that may be outside of the buffer array size
 * 
 * Outputs:
 *  idx - equivalent index, but guaranteed to be within buffer array size
 */
int wrapBufferIdx(int idx) {
    const int NUM_TRIES = 10; // max iterations (prevent infinite loop)

    for (int j = 0; j < NUM_TRIES; j++) {
        if (idx < 0) {                  // wrap index if it is negative
            idx = idx + BUFFERSIZE;     // -1 becomes BUFFERSIZE - 1
        } else if (idx >= BUFFERSIZE) { // wrap index if it is too large
            idx -= BUFFERSIZE;          // BUFFERSIZE + 1 becomes 1
        } else {                        // within valid bounds for indexing buffer
            return idx;
        }
    }
    Serial.print("wrapBufferIdx failed to find valid index after ");
    Serial.print(NUM_TRIES);
    Serial.println(" tries.");
    Serial.println("Defaulting to an index of 0 (Timing module)");
    
    // Return -1 to throw an error since something has gone very wrong if we get here
    return -1;
}

/* - - - - - - checkSweepChange - - - - - - *
 * Usage:
 *  determine if the ADCS should change its sweep direction
 *  invoke event to be used by ADCS module if direction change is necessary
 * 
 * Inputs:
 *  buffer - array of photodiode voltages
 *  bufIdx - index of most recent photodiode measurement
 * 
 * Outputs:
 *  none
 */
void checkSweepChange(uint16_t buffer[], int bufIdx) {
    // get the most recent smoothed photodiode voltage
    float pdNew = voltageRunningMean(buffer, bufIdx);

    // subtract the proper number of indices to get the next most recent photodiode voltage
    float pdOld = voltageRunningMean(buffer, wrapBufferIdx(bufIdx - ADCS_SWEEP_IDX_OFFSET));

    // change the sweep direction if the voltage crossed the threshold
    //  requires optic to have recently dropped below the voltage threshold 
    if ((pdNew < SUN_THRESH_VOLTAGE) && (pdOld > SUN_THRESH_VOLTAGE)){
        scienceMode.sweepChange();
    }
}

/* - - - - - - TODO: recoverPayloadMode - - - - - - *
 * Usage:
 *  Called in init routine to determine the payload mode after an unexpected shutdown
 *  would be useful to implement in the future if the payload mode list becomes more complicated
 *  but for now the payload mode is just initialized as STANDBY_MODE in the constructor
 *  which is sufficient for testing at this stage
 *  If an unexpected shutdown were to happen at this point, returning to STANDBY_MODE is 
 *  always the proper response, as this will result in a maximum of two lost data windows
 * 
 * Inputs:
 * 
 * Outputs:
 *  mode of payload
 */