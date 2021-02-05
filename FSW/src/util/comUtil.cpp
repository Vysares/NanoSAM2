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

static CommandQueue commandQueue();


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
    int newCommandCode = getCommand();
    
    if (newCommandCode != -1) // if the new command is valid, add it to the queue
    {
        queueCommand(newCommandCode);
    }

}


int getCommand();
{
    // read Serial
    if (Serial.available() > 0) // check if there is serial input
    {
        int serialInput = Serial.parseInt(); // read the command
        if ((serialInput <= static_cast<int>(Command::DoNothing)) && (serialInput >= 0)) // check if command is valid
        {
            Serial.println("Command Recieved.")
            return serialInput;
        }
        else
        {
            Serial.println("Invalid command recieved.");
            Serial.println("(Command Handling)");
        }
    }
    return -1; // no command

}

void queueCommand(int commandCode)
{

}

void executeCommand(int commandCode)
{
    switch (commandCode)
    {
        case EnterSafeMode:
        {
            
        }
    }
}
