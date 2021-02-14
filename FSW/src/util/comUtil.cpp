/* comUtil.cpp handles NS2 payload commands
 * Usage:
 *  Reads incoming commands from serial, adds them to the command queue
 *  and executes each command. 
 *      
 * 
 * Modules encompassed:
 *  Command Handling
 *
 * Additional files needed for compilation:
 *  config.hpp
 *  comUtil.hpp
 *  
 */

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries

// NS2 headers
#include "../headers/config.hpp"
#include "../headers/comUtil.hpp"


/* Module Variable Definitions */
int commandQueue[COMMAND_QUEUE_SIZE] = {};  // initializes as all zeros
static int queueIndex = 0;                  // next available index of command queue
static bool isPaused = false;               // indicates if command execution is paused

/* - - - - - - Module Driver Functions - - - - - - */

/* - - - - - - commandHandling - - - - - - *
 * Usage:
 *  looks for and reads incoming commands via serial, adds them to the command queue, and executes
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
    int newCommand = readCommand(); // read command from serial
    if (checkMetaCommand(newCommand)) // if new command is a meta command, execute it immediately
    {
        executeCommand(newCommand);
    }
    else if (newCommand != -1) // if the new command is valid, add it to the queue
    {
        queueCommand(newCommand);
    }
    executeAllCommands(); // execute all commands in the queue
}

/* - - - - - - Helper Functions - - - - - - */

/* - - - - - - readCommand - - - - - - *
 * Usage:
 *  Reads serial input for an incoming command, checks if it has recieved a valid command,
 *  and returns the new command
 * 
 * Inputs:
 *  none
 * 
 * Outputs:
 *  a single valid command code, or -1 if no valid code is recieved
 */
int readCommand()
{
    // read Serial
    if (Serial.available() > 0) // check if there is serial input
    {
        int serialInput = Serial.parseInt(); // read the command
        if ((serialInput <= static_cast<int>(Command::DO_NOTHING)) && (serialInput > 0)) // check if command is valid
        {
            //Serial.println("Command Recieved."); // for debugging
            return serialInput;
        }
        else
        {
            // print warning, indicating the invalid command and the range of valid commands
            Serial.print("Invalid command recieved: ");
            Serial.println(serialInput);
            Serial.print("Valid commands are between 1 and ");
            Serial.println(static_cast<int>(Command::DO_NOTHING));
            Serial.println("(Command Handling)");
        }
    }
    return -1; // no command
}

/* - - - - - - queueCommand - - - - - - *
 * Usage:
 *  Adds command to the next availiable spot in the command queue.
 *  New commands are not added if the queue is full.
 * 
 * Inputs:
 *  command - a single command code
 * 
 * Outputs:
 *  None
 */
void queueCommand(int command)
{
    if (queueIndex < COMMAND_QUEUE_SIZE) // if end of queue has not been reached, add command to queue 
    {
        commandQueue[queueIndex] = command;
        queueIndex++;
    }
    else
    {
        Serial.println("Command queue full.");
        Serial.println("(Command Handling)");
    }
}

/* - - - - - - checkMetaCommand - - - - - - *
 * Usage:
 *  Checks if input command is a meta command.
 *  Meta commands are executed immediately even if the queue is paused
 * 
 * Inputs:
 *  command - a single command code
 * 
 * Outputs:
 *  True if meta command detected
 *  False if no meta command detected
 */
bool checkMetaCommand(int command)
{
    command = static_cast<Command>(command);
    switch (command)
    {
        case PAUSE_EXECUTE_COMMANDS: 
            break;
        case RESUME_EXECUTE_COMMANDS:
            break;
        case CLEAR_COMMAND_QUEUE:
            break;
        default:
            return false; // no meta command detected
            break;
    }
    return true;
}

/* - - - - - - executeAllCommands - - - - - - *
 * Usage:
 *  Executes every command in the command queue, starting from index 0 and up to the current index
 * 
 * Inputs:
 *  None 
 * 
 * Outputs:
 *  None
 */
void executeAllCommands()
{
    if (!isPaused) // if command execution is not paused
    {
        for (int i = 0; i < queueIndex; i++) // execute each command in the queue
        {
            executeCommand(commandQueue[i]);
        }
        clearCommandQueue();
    }
}

/* - - - - - - clearCommandQueue - - - - - - *
 * Usage:
 *  Sets all elements in command queue to 0
 *  resets queue index to 0.
 * 
 * Inputs:
 *  None 
 * 
 * Outputs:
 *  None
 */
void clearCommandQueue()
{
    for (int i = 0; i < COMMAND_QUEUE_SIZE; i++) // execute each command in the queue
        {
            commandQueue[i] = 0;
        }
        queueIndex = 0; // reset queue index
}

/* - - - - - - executeCommand - - - - - - *
 * Usage:
 *  executes a single command by checking it against the Command enum
 * 
 * Inputs:
 *  command - a single command code
 * 
 * Outputs:
 *  None
 */
void executeCommand(int command)
{
    // When adding new commands, make sure to include any relevant headers!
    command = static_cast<Command>(command); // cast command as Command enum
    switch (command)
    {
        // Mode change
        case ENTER_SAFE_MODE: 
            scienceMode.setMode(SAFE_MODE);
            Serial.println("Command Recieved - Entering Safe Mode. This mode must be manually exited.");
            break;
        
        case ENTER_STANDBY_MODE: 
            scienceMode.setMode(STANDBY_MODE);
            Serial.println("Command Recieved - Entering Standby Mode. This mode must be manually exited.");
            break;
        
        case ENTER_SUNSET_MODE: 
            scienceMode.setMode(SUNSET_MODE);
            Serial.println("Command Recieved - Entering Sunset Mode.");
            break;
        
        case ENTER_PRE_SUNRISE_MODE: 
            scienceMode.setMode(PRE_SUNRISE_MODE);
            Serial.println("Command Recieved - Entering Pre-Sunrise Mode.");
            break;
        
        case ENTER_SUNRISE_MODE: 
            sunriseTimerEvent.start(); // sunrise mode will never end w/o this call
            scienceMode.setMode(SUNRISE_MODE);
            Serial.println("Command Recieved - Entering Sunrise Mode.");
            break;
        
        // Housekeeping
        case DISABLE_WD_RESET: 
            // TODO: change WD timer value to >1.2 sec by calling setDuration()
            break;
        
        case HEATER_ON: 
            // TODO: call corresponding function in housekeeping module
            break;
        
        case HEATER_OFF: 
            // TODO: call corresponding function in houskeeping module
            break;
        
        // Command Handling
        case PAUSE_EXECUTE_COMMANDS: 
            isPaused = true;
            Serial.println("Command execution paused.");
            break;
        
        case RESUME_EXECUTE_COMMANDS: 
            isPaused = false;
            Serial.println("Command execution resumed.");
            break;
        
        case CLEAR_COMMAND_QUEUE: 
            clearCommandQueue();
            Serial.println("Command queue cleared.");
            break;

        case ADCS_POINTING_AT_SUN_T:
            scienceMode.setPointingAtSun(true);
            Serial.println("Command Recieved - ADCS_POINTING_AT_SUN set to true.");
            break;

        case ADCS_POINTING_AT_SUN_F:
            scienceMode.setPointingAtSun(false);
            Serial.println("Command Recieved - ADCS_POINTING_AT_SUN set to false.");
            break;

        case EXIT_MAIN_LOOP:
            scienceMode.exitMainLoopEvent.invoke();
            Serial.println("Command Recieved - Exiting main loop at next opportunity.");
            break;

        // End of list
        case DO_NOTHING: 
            Serial.println("Doing nothing.");
            break;
        
        default: 
            Serial.println("Code in command queue has no matching command (this should not happen).");
            Serial.println("(Command Handling)");
            break;
    }
}
