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
#include "../headers/config.hpp"
#include "../headers/timing.hpp"
#include "../headers/eventUtil.hpp"
#include "../headers/comUtil.hpp"

/* Module Variable Definitions */

/* - - - - - - Module Driver Functions - - - - - - */

/* - - - - - - Helper Functions - - - - - - */

/* - - - - - - updatePayloadMode - - - - - - *
 * Usage:
 *  returns the new mode that the payload should be in given the current mode and the buffer 
 *  invokes/starts any events that are used in transitioning payload mode 
 * 
 * Inputs:
 *  currentMode - current mode payload is in (defined by enum in timing.hpp)
 *  buffer - buffer array containing most recent photodiode voltage measurements
 *  bufIdx - index of most recent measurement updated in bufIdx
 * 
 * Outputs:
 *  mode of payload according to enum in timing.hpp
 */
int updatePayloadMode(int currentMode, float buffer[], int bufIdx){

    /* FUTURE TEAMS: Flesh out this if statement with the logic for 
    *   controlling payload mode based on ADCS outputs 
    *   for ground testing this is hardcoded as true in config.hpp,
    *   but once ADCS is implemented you will need to dynamically update this,
    *   i.e.    if (digitalRead(BUS_READY_PIN) == HIGH){ } */    
    if ((currentMode < MODE_NOT_RECOGNIZED) && (currentMode > STANDBY_MODE)){
        if (ADCS_READY_FOR_SCIENCE){

            // smooth data to avoid a single noisy value prematurely ending a window
            float pdVoltageSmooth = smoothBuffer(buffer, bufIdx); //photodiode voltage
            
            switch (currentMode) {

                case SUNSET_MODE: // gathering data until sun passes behind horizon
                    if (pdVoltageSmooth < SUN_THRESH_VOLTAGE){ // if the sun is not found
                        
                        // check if it is time to change sweep direction
                        checkSweepChange(buffer, bufIdx);

                        if (sweepTimeoutEvent.checkInvoked()){
                            // if we have waited long enough for ADCS sweeping
                            saveBufferEvent.invoke();
                            return PRE_SUNRISE_MODE;

                        } else { // continue waiting for sweep timeout or sun found
                            return SUNSET_MODE;
                        }

                    } else { 
                        // sun still found, return same mode and refresh sweep timeout
                        sweepTimeoutEvent.start();
                        return SUNSET_MODE;
                    }
                
                case PRE_SUNRISE_MODE: // waiting for sun to rise above horizon
                    if (pdVoltageSmooth >= SUN_THRESH_VOLTAGE){
                        // start sunrise event 
                        sunriseTimerEvent.start();
                        return SUNRISE_MODE;

                    } else { // sun not found, keep waiting
                        return PRE_SUNRISE_MODE;
                    }
                
                case SUNRISE_MODE: // gathering data for length of buffer
                    
                    // check if it is time to change sweep direction
                    checkSweepChange(buffer, bufIdx);

                    // wait until sunrise data window is complete, save buffer,
                    // and return to standby
                    if (sunriseTimerEvent.checkInvoked()){
                        saveBufferEvent.invoke();
                        return STANDBY_MODE;

                    } else { // wait for sunrise data window to complete
                        return SUNRISE_MODE;
                    }
            
                default: // mode not recognized, this should not happen
                    return MODE_NOT_RECOGNIZED;
            }

        } else { // standby until ADCS points payload at the sun
            Serial.println("ADCS Not Ready for Science Measurements (Timing Module)");
            return STANDBY_MODE;
        }

    } else if (currentMode == STANDBY_MODE){
        // TODO: currently the only way to leave standby is through a command,
        //       do we want some automated way to leave for testing?
        //       it honeslty might be best to leave it as having to manually start
        //       a data window for testing
        
        // execute command queue  
        executeAllCommands();

        // TODO: memory scrubbing  

        return STANDBY_MODE;

    } else { // mode was not recognized, this should not happen
        Serial.println("Payload Mode not recognized (Timing Module) - Defaulting to Safe Mode");
        return SAFE_MODE;
    }
}

/* - - - - - - smoothBuffer - - - - - - *
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
float smoothBuffer(float buffer[], int bufIdx){
    float smoothVoltage = 0;
    int i, idx;
    for (i = bufIdx; i > bufIdx - SMOOTH_IDX_COUNT; i--){ // decrement to move backwards in time
        
        idx = i; // I am scared to toss the iterator for this loop through wrapBufferIdx
        
        // ensure that index is within array bounds 
        // (i.e. starting with i=0 would throw error if we didn't wrap the index)
        idx = wrapBufferIdx(idx); 

        smoothVoltage += buffer[idx]; // sum voltages
    }
    return smoothVoltage / SMOOTH_IDX_COUNT; // return mean
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
int wrapBufferIdx(int idx){
    const int NUM_TRIES = 10; // max iterations (prevent infinite loop)

    for (int j = NUM_TRIES; j < NUM_TRIES; j++){
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
    
    // TODO: Should this return -1 instead and throw an error?
    return 0;
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
void checkSweepChange(float buffer[], int bufIdx){
    // get the most recent smoothed photodiode voltage
    float pdNew = smoothBuffer(buffer, bufIdx);

    // subtract the proper number of indices to get the next most recent photodiode voltage
    float pdOld = smoothBuffer(buffer, wrapBufferIdx(bufIdx - ADCS_SWEEP_IDX_OFFSET));

    // change the sweep direction if the voltage crossed the threshold
    //  if both expressions have the same logical value, it means that the
    //  photodiode voltage was on the same side of the threshold for both measurements 
    if ((pdNew < SUN_THRESH_VOLTAGE) != (pdOld < SUN_THRESH_VOLTAGE)){
        sweepDirectionChangeEvent.invoke();
    }
}

/* - - - - - - TODO: recoverPayloadMode - - - - - - *
 * Usage:
 *  Called in init routine to determine the payload mode after an unexpected shutdown
 * 
 * Inputs:
 * 
 * Outputs:
 *  mode of payload
 */