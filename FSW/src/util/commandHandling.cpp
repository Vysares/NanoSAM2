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
// All libraries are put in commandHandling.hpp
// NS2 headers
#include "../headers/commandHandling.hpp"
#include "../headers/dataCollection.hpp"
#include "../headers/timing.hpp" //for mode enum
#include "../headers/faultManager.hpp"


/* Module Variable Definitions */
static int commandQueue[COMMAND_QUEUE_SIZE] = {};  // initializes as all zeros
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
void commandHandling() {
    int newCommand = readCommand(); // read command from serial
    if (checkMetaCommand(newCommand)) { // if new command is a meta command, execute it immediately
        executeCommand(newCommand);
    } else if (newCommand != -1) { // if the new command is valid, add it to the queue
        queueCommand(newCommand);
    }
    executeAllCommands(); // execute all commands in the queue
}

/* - - - - - - Helper Functions - - - - - - */

/* - - - - - - readCommand - - - - - - *
 * Usage:
 *  Reads serial input for an incoming command, checks if it has received a valid command,
 *  and returns the new command
 * 
 * Inputs:
 *  none
 * 
 * Outputs:
 *  a single valid command code, or -1 if no valid code is received
 */
int readCommand() {
    // read Serial
    if (Serial.available() > 0) { // check if there is serial input
        int serialInput = Serial.parseInt(); // read the command
        if ((serialInput <= commandCode::DO_NOTHING) && (serialInput > 0)) { // check if command is valid
            Serial.print("Command Received via Serial - code: ");
            Serial.println(serialInput);
            return serialInput;
        } else {
            // print warning, indicating the invalid command and the range of valid commands
            Serial.print("Invalid command received: ");
            Serial.println(serialInput);
            Serial.print("Valid commands are between 1 and ");
            Serial.println(commandCode::DO_NOTHING);
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
void queueCommand(int command) {
    if (queueIndex < COMMAND_QUEUE_SIZE) { // if end of queue has not been reached, add command to queue 
        commandQueue[queueIndex] = command;
        queueIndex++;
    } else {
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
bool checkMetaCommand(int command) {
    switch (command) {
        case commandCode::PAUSE_EXECUTE_COMMANDS: 
            break;
        case commandCode::RESUME_EXECUTE_COMMANDS:
            break;
        case commandCode::CLEAR_COMMAND_QUEUE:
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
void executeAllCommands() {
    if (!isPaused) { // if command execution is not paused
        for (int i = 0; i < queueIndex; i++) { // execute each command in the queue
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
void clearCommandQueue() {
    for (int i = 0; i < COMMAND_QUEUE_SIZE; i++) { // execute each command in the queue
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
void executeCommand(int command) {
    // When adding new commands, make sure to include any relevant headers!
    switch (command) {
        // Mode change
        case commandCode::ENTER_SAFE_MODE: 
            scienceMode.setMode(SAFE_MODE);
            Serial.println("Command Executed - Entering Safe Mode. This mode must be manually exited.");
            break;
        
        case commandCode::ENTER_STANDBY_MODE: 
            scienceMode.setMode(STANDBY_MODE);
            Serial.println("Command Executed - Entering Standby Mode. This mode must be manually exited.");
            break;
        
        case commandCode::ENTER_SUNSET_MODE: 
            scienceMode.setMode(SUNSET_MODE);
            Serial.println("Command Executed - Entering Sunset Mode.");
            break;
        
        case commandCode::ENTER_PRE_SUNRISE_MODE: 
            scienceMode.setMode(PRE_SUNRISE_MODE);
            Serial.println("Command Executed - Entering Pre-Sunrise Mode.");
            break;
        
        case commandCode::ENTER_SUNRISE_MODE: 
            sunriseTimerEvent.start(); // sunrise mode will never end w/o this call
            scienceMode.setMode(SUNRISE_MODE);
            Serial.println("Command Executed - Entering Sunrise Mode.");
            break;
        
        // Housekeeping
        case commandCode::DISABLE_WD_RESET: 
            // TODO: change wd timer duration by calling setDuration()
            break;
        
        case commandCode::HEATER_ON: 
            digitalWrite(PIN_HEAT, HIGH);
            Serial.println("Command Executed - Heater turned ON. Automatic heater control still has priority.");
            break;
        
        case commandCode::HEATER_OFF: 
            digitalWrite(PIN_HEAT, LOW);
            Serial.println("Command Executed - Heater turned OFF. Automatic heater control still has priority.");
            break;
        
        case commandCode::FORCE_HEATER_ON_T:
            FORCE_HEATER_ON = true;
            Serial.println("Command Executed - Heater is now forced ON. Automatic heater control has been disabled.");
            break;

        case commandCode::FORCE_HEATER_ON_F:
            FORCE_HEATER_ON = false;
            Serial.println("Command Executed - Automatic heater control enabled.");
            break;

        case commandCode::CALIBRATE_OPTICS_THERM:
            OPTICS_THERM_CAL_VOLTAGE = TEENSY_VOLTAGE_RES*analogRead(PIN_OPTICS_THERM);
            Serial.print("Command Executed - Optics thermistor calibrated at ");
            Serial.print(OPTICS_THERM_CAL_TEMP);
            Serial.println(" C");
            break;

        // Command Handling
        case commandCode::PAUSE_EXECUTE_COMMANDS: 
            isPaused = true;
            Serial.println("Command Executed - Command execution paused.");
            break;
        
        case commandCode::RESUME_EXECUTE_COMMANDS: 
            isPaused = false;
            Serial.println("Command Executed - Command execution resumed.");
            break;
        
        case commandCode::CLEAR_COMMAND_QUEUE: 
            clearCommandQueue();
            Serial.println("Command Executed - Command queue cleared.");
            break;

        // ADCS
        case commandCode::ADCS_POINTING_AT_SUN_T:
            scienceMode.setPointingAtSun(true);
            Serial.println("Command Executed - ADCS_POINTING_AT_SUN set to true.");
            break;

        case commandCode::ADCS_POINTING_AT_SUN_F:
            scienceMode.setPointingAtSun(false);
            Serial.println("Command Executed - ADCS_POINTING_AT_SUN set to false.");
            break;

        // Memory
        case commandCode::DOWNLINK_START:
            downlinkEvent.invoke();
            Serial.println("Command Executed - Downlink will begin when payload enters standby.");
            break;

        case commandCode::SCRUB_FLASH:
            scrubEvent.invoke();
            Serial.println("Command Executed - Flash scrub initiated.");
            break;

        // Fault Mitigation
        case commandCode::WIPE_EEPROM:
            wipeEEPROM();
            Serial.println("Command Executed - Wipe EEPROM");
            break;

        case commandCode::RESET_PERSISTENT_DATA:
            resetPersistentData();
            Serial.println("Command Executed - Persistent data cleared");
            break;


        case commandCode::ACT_ON_FAULTS_T:
            ACT_ON_NEW_FAULTS = true;
            Serial.println("Command Executed - NS2 will attempt to correct faults");
            break;

        case commandCode::ACT_ON_FAULTS_F:
            ACT_ON_NEW_FAULTS = false;
            Serial.println("Command Executed - NS2 will NOT attempt to correct faults");
            break;

        // Main Loop
        case commandCode::EXIT_MAIN_LOOP:
            exitMainLoopEvent.invoke();
            Serial.println("Command Executed - Exiting main loop at next opportunity.");
            break;

        // End of list
        case commandCode::DO_NOTHING: 
            Serial.println("Command Executed - Do nothing.");
            break;
        
        default: 
            Serial.println("Code in command queue has no matching command (this should not happen).");
            Serial.println("(Command Handling)");
            break;
    }
}
