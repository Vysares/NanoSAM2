#ifndef COMUTIL_H
#define COMUTIL_H 

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries
#include <Arduino.h>

// NS2 headers
// (all of them)
#include "config.hpp"
#include "memUtil.hpp"
#include "timing.hpp" //for mode enum

/* - - - - - - Declarations - - - - - - */
void commandHandling();
int readCommand();
void queueCommand(int command);
bool checkMetaCommand(int command);
void executeAllCommands();
void clearCommandQueue();
void executeCommand(int command);

enum Command // all possible commands
{
    // commands are 1 indexed so that the default initializer can be used for the command queue

    // Mode change
    ENTER_SAFE_MODE = 1,          // enter safemode state
    ENTER_STANDBY_MODE,           // enter standby state
    ENTER_SUNSET_MODE,            // enter sunset data collection mode
    ENTER_PRE_SUNRISE_MODE,       // enter watch-for-sunset mode
    ENTER_SUNRISE_MODE,           // enter sunrise data collection mode
    
    // Housekeeping
    DISABLE_WD_RESET,             // disable watchdog reset signal, forcing a restart
    HEATER_ON,                    // turn heater on, does not override housekeeping heater control
    HEATER_OFF,                   // turn heater off, does not override housekeeping heater control

    // Command Handling
    PAUSE_EXECUTE_COMMANDS,       // pause command execution
    RESUME_EXECUTE_COMMANDS,      // resume command execution
    CLEAR_COMMAND_QUEUE,          // clears all commands from queue
    
    // End of list
    DO_NOTHING                    // do nothing. KEEP THIS LAST IN THE ENUM, it is used for indexing.
};

#endif
