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
#include "src/headers/comUtil.hpp"
#include "src/headers/eventUtil.hpp"
#include "src/headers/timingClass.hpp"
#include "src/headers/timing.hpp"
#include "src/headers/memUtil.hpp"

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
bool init(){
    scienceMode.setPointingAtSun(true);
    scienceMode.setMode(SAFE_MODE); // this is done in the class constructor so we may not need it

    // lockout duration has to be initialized with a bad value for compilation, overwrite
    //    it here with a value that is a function of existing vars in config.hpp
    scienceMode.sweepChangeLockout.setDuration(ADCS_SWEEP_CHANGE_DURATION);

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
int main()
{
    //carry out initializations
    if (!init()){
        Serial.println("Initialization failed, exiting main loop");
        return -1;
    }
    
    // TODO: Change main loop to account for critical exit conditions
    while(true) // run main loop forever
    {
        // execute commands  
        commandHandling();

        // test multiple file structure by calling functions from memUtil.cpp
        if (scienceMode.getMode() != SAFE_MODE){
            scienceMemoryHandling();
        }
    }

    return 0;
}
