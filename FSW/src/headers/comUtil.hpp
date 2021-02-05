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


/* - - - - - - Declarations - - - - - - */

// TODO: put this in config file

enum Command
{
    // Mode change
    EnterSafeMode,              // enter safemode state
    EnterStandbyMode,           // enter standby state
    EnterSunsetMode,            // enter sunset data collection mode
    EnterPreSunriseMode,        // enter watch-for-sunset mode
    EnterSunriseMode,           // enter sunrise data collection mode
    
    // Memory handling
    ForceSaveBuffer,            // save the buffer contents to flash immediately
    EraseFlash,                 // wipe the flash modules

    // System
    DisableWDReset,             // disable watchdog reset signal, forcing a restart
    HeaterOn,                   // turn heater on
    HeaterOff,                  // turn heater off
    StartCommandSequence,       // pause command execution
    ExecuteAllCommands,         // execute all commands in the queue
    
    
    DoNothing                  // this command does nothing. KEEP THIS LAST IN THE ENUM!
};

int commandHandling();

class CommandQueue
{
    public:
        int queue[COMMAND_QUEUE_SIZE];

        CommandQueue();
};


#endif
