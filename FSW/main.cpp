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
#include "src/headers/eventUtil.hpp"
#include "src/headers/timingClass.hpp"
#include "src/headers/timing.hpp"
#include "src/headers/dataCollection.hpp"

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
    Serial.print("Initializing NanoSAM II FSW... ");

    scienceMode.setPointingAtSun(true);
    scienceMode.setMode(STANDBY_MODE); // this is done in the class constructor so we may not need it

    // lockout duration has to be initialized with a default value during compilation,
    //    set it to the proper macro from config.hpp
    scienceMode.sweepChangeLockout.setDuration(ADCS_SWEEP_CHANGE_DURATION);


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
int main()
{
    //carry out initializations
    if (!init()){
        Serial.println("Initialization failed, exiting main loop");
        return -1;
    }
    
    while(true) // run main loop until exit command
    {
        // execute commands  
        commandHandling();

        // test multiple file structure by calling functions from memUtil.cpp
        if (scienceMode.getMode() != SAFE_MODE){
            scienceMemoryHandling();
        }

        if (scienceMode.exitMainLoopEvent.checkInvoked()){
            Serial.println("Exiting main loop");
            break;
        }
    }

    return 0;
}
