/* main.cpp is the main script for the NanoSAM II FSW 
 * Usage:
 *  run this script through Teensyduino to power the NS2 payload
 * 
 * Additional files needed for compilation:
 *  memUtil.hpp
 *  memUtil.cpp
 *
 */

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries

// NS2 headers
#include "src/headers/config.hpp"
#include "src/headers/commandHandling.hpp"
#include "src/headers/housekeeping.hpp"
#include "src/headers/timingClass.hpp"
#include "src/headers/timing.hpp"
#include "src/headers/dataCollection.hpp"
#include "src/headers/faultManager.hpp"

/* - - - - - - Functions - - - - - - */

/* - - - - - - init - - - - - - *
 * Usage:
 *  initializes NanoSAM II FSW
 * 
 * Inputs:
 *  none
 *  
 * Outputs:
 *  initStatus - flag (true if initialization was successful)
 */
bool init() {
    Serial.print("Initializing NanoSAM II FSW... ");
    
    // feed the dog
    feedWD();

    // load persistent data from EEPROM
    loadEEPROM();
    recordNewStart();

    // Set pin modes
    pinMode(PIN_HEAT, OUTPUT);
    pinMode(PIN_WD_RESET, OUTPUT);
    pinMode(PIN_ADC_CS, OUTPUT);
    pinMode(PIN_AREG_CURR, INPUT);
    pinMode(PIN_DREG_CURR, INPUT);
    pinMode(PIN_DREG_PG, INPUT);
    pinMode(PIN_PHOTO, INPUT);
    pinMode(PIN_DIGITAL_THERM, INPUT);
    pinMode(PIN_ANALOG_THERM, INPUT);
    pinMode(PIN_OPTICS_THERM, INPUT);

    // setup serial
    Serial.begin(SERIAL_BAUD);
    Serial.setTimeout(SERIAL_TIMEOUT_MSEC);

    scienceMode.setPointingAtSun(true);
    scienceMode.setMode(STANDBY_MODE); // this is done in the class constructor so we may not need it

    // lockout duration has to be initialized with a default value during compilation,
    //    set it to the proper macro from config.hpp
    scienceMode.sweepChangeLockout.setDuration(ADCS_SWEEP_CHANGE_DURATION);

    // Begin timers
    dataProcessEvent.start();
    wdTimer.start();
    housekeepingTimer.start();
    

    Serial.println("Complete");
    return true;
}

/* - - - - - - main - - - - - - *
 * Usage:
 *  Main function entered on runtime, fill with NS2 functionality
 * 
 * Inputs:
 *  none
 *  
 * Outputs:
 *  None
 */
int main() {
    //carry out initializations
    if (!init()) {
        Serial.println("Initialization failed, exiting main loop");
        return -1;
    }
    static unsigned long mainLoopIterations = 0; // counter for loop iterations
    
    /* = = = = = = = = = = = = = = = = =
     * = = = = = = Main Loop = = = = = = 
     * = = = = = = = = = = = = = = = = = */
    while(true) { // run main loop until exit command
        
        /* ===== ALWAYS EXECUTE ===== */
        commandHandling(); // handle commands
                                            
        if (housekeepingTimer.checkInvoked()) { // housekeeping
            handleHousekeeping(); 
        }

        if (wdTimer.checkInvoked()) { // feed the watchdog
            feedWD(); 
        }

        /* ===== ONLY EXECTUTE DURING NORMAL OPERATION ===== */
        if (scienceMode.getMode() != SAFE_MODE) {
            scienceMemoryHandling(); // handle science data collection
            handleFaults();          // handle faults
        }

         /* ===== ONLY EXECUTE ON ENTRY TO STANDBY MODE ===== */
        if (scienceMode.onStandbyEntry.checkInvoked()) {
            resetFaultCounts(); // reset fault occurence counts
         }

         /* ===== ONLY EXECUTE IN STANDBY MODE ===== */
        if (scienceMode.getMode() == STANDBY_MODE) {
            if (scrubEvent.checkInvoked()) { // scrub flash 
                scrubFlash(); 
            }
            if (downlinkEvent.checkInvoked()) { // downlink files
                downlink(); 
            }
        }

         /* ===== EXIT MAIN LOOP ===== */
        if (exitMainLoopEvent.checkInvoked()) { // exit main loop
            prepareForRestart();
            Serial.print("Exiting main loop at iteration ");
            Serial.println(mainLoopIterations);
            break;
        }
    }

    return 0;
}
