/* hkUtil.cpp handles NS2 payload housekeeping
 * Usage:
 *  module functionality
 *      system state data collection
 *      temperature control
 * 
 *  function definitions
 *  put function declarations in memUtil.hpp
 * 
 * Modules encompassed:
 *  
 *
 * Additional files needed for compilation:
 *  none
 */

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries

// NS2 headers
#include "../headers/comUtil.hpp"

int command[commandQueueSize];


/* - - - - - - Module Driver Functions - - - - - - */

/* - - - - - - commandHandling - - - - - - *
 * Usage:
 *  looks for and reads incoming commands, adds them to the command queue, and executes
 *  each command in the queue
 * 
 * Inputs:
 *  none
 *  
 * Outputs:
 *  None
 */
void commandHandling()
{
    int newCommand = getCommand();
    newCommand
}



byte getCommand();
{
    // read Serial
    if (Serial.available() > 0) // check if there is serial input
    {
        int serialInput = Serial.parseInt();
        return serialInput;
    }
    return 0; // no command

}

void parseCommand(byte command)
{
    switch newCommand
    {

    }
}
