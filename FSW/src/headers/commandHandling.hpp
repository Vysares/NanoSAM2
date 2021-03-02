#ifndef COMHANDLING_H
#define COMHANDLING_H

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries

// NS2 headers
#include "config.hpp"

/* - - - - - - Declarations - - - - - - */
void commandHandling();
int readCommand();
void queueCommand(int command);
bool checkMetaCommand(int command);
void executeAllCommands();
void clearCommandQueue();
void executeCommand(int command);

enum Command { // all possible commands
    // commands are 1 indexed so that the default initializer can be used for the command queue

    // Mode change
    ENTER_SAFE_MODE = 1,            // enter safemode state
    ENTER_STANDBY_MODE,             // enter standby state
    ENTER_SUNSET_MODE,              // enter sunset data collection mode
    ENTER_PRE_SUNRISE_MODE,         // enter watch-for-sunset mode
    ENTER_SUNRISE_MODE,             // enter sunrise data collection mode
    
    // Housekeeping
    DISABLE_WD_RESET,               // disable watchdog reset signal, forcing a restart
    HEATER_ON,                      // turn heater on, does not override housekeeping heater control
    HEATER_OFF,                     // turn heater off, does not override housekeeping heater control
    FORCE_HEATER_ON_T,              // force the heater on, overrrides houskeeping heater control
    FORCE_HEATER_ON_F,              // stop forcing the heater on, giving control back to housekeeping
    CALIBRATE_OPTICS_THERM,         // take new voltage measurement at known temp for optics thermistor

    // Command Handling
    PAUSE_EXECUTE_COMMANDS,         // pause command execution
    RESUME_EXECUTE_COMMANDS,        // resume command execution
    CLEAR_COMMAND_QUEUE,            // clears all commands from queue
    
    // ADCS
    ADCS_POINTING_AT_SUN_T,         // sets pointing at sun flag to true
    ADCS_POINTING_AT_SUN_F,         // sets pointing at sun flag to false

    // Memory
    DOWNLINK_START,                // start downlink at next available time
    SCRUB_FLASH,                   // start scrubbing the flash memory for errors

    // Main Loop Commands
    EXIT_MAIN_LOOP,

    // End of list
    DO_NOTHING                    // do nothing. KEEP THIS LAST IN THE ENUM, it is used for indexing.
};

#endif
