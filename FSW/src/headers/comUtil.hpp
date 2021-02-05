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
static int commandQueueSize = 100;

enum Command
{
    EnterSafemode, // enter safemode state
    EnterStandby, // enter standby state
    EnterDataCollection, // enter data collection mode
    disableWDReset, // dsiable watchdog reset signal, forcing a restart
    
    HeaterOn, // turn heater on
    HeaterOff, // turn heater off
    StartCommandSequence, // pause command execution
    ExecuteAllCommands, // execute all commands in the queue
}

int commandHandling();

#endif
