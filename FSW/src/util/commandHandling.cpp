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
#include "../headers/housekeeping.hpp"


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
 *  None
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
        case commandCode::INFO:
        case commandCode::PAUSE_EXECUTE_COMMANDS: 
        case commandCode::RESUME_EXECUTE_COMMANDS:
        case commandCode::CLEAR_COMMAND_QUEUE:
            break;
        default:
            return false; // no meta command detected
            break;
    }
    return true;
}

/* - - - - - - checkIfCommandAllowed - - - - - - *
 * Usage:
 *  Checks if input command is allowed to be executed.
 * 
 * Inputs:
 *  command - a single command code
 * 
 * Outputs:
 *  True if command is allowed
 *  False if command is not allowed
 */
bool checkIfCommandAllowed(int command) {
    if (DANGER_COMMANDS_ALLOWED) { return true; }

    switch (command) {
        // list dangerous commands here
        case commandCode::CALIBRATE_OPTICS_THERM:
        case commandCode::RESET_PERSISTENT_DATA:
        case commandCode::WIPE_EEPROM:
        case commandCode::DISABLE_WD_RESET:
        case commandCode::EXIT_MAIN_LOOP:
            break;
        default:
            return true; // no dangerous command detected
            break;
    }
    Serial.println("!!! This command can cause shutdown or data loss !!!");
    Serial.println("You must enable potentially dangerous commands to execute this command.");
    return false;
}

/* - - - - - - executeAllCommands - - - - - - *
 * Usage:
 *  Executes every command in the command queue, starting from index 0 and up to the current index
 * 
 * Inputs:
 *  None 
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
    if (!checkIfCommandAllowed(command)) { return; }

    // When adding new commands, make sure to include any relevant headers!
    switch (command) {
        // Info
        case commandCode::INFO:
            printInfo();
            break;

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

        // Data Collection
        case commandCode::SAVE_BUFFER:
            if (saveBuffer()) { Serial.println("Command Executed - Buffer saved to flash."); }
            else { Serial.println("Command Executed - Attempt to save buffer failed."); }
            break;

        case commandCode::DOWNLINK_START:
            downlinkEvent.invoke();
            if (scienceMode.getMode() == STANDBY_MODE) { Serial.println("Command Executed - Downlink initiated."); }
            else { Serial.println("Command Executed - Downlink will begin when payload enters standby."); }
            break;

        case commandCode::STREAM_PHOTO_SPI_T:
            STREAM_PHOTO_SPI = true;
            Serial.println("Command Executed - Transmitting photodiode voltages read by SPI in real time.");
            Serial.println("PHOTO_SPI | time (ms) | photodiode voltage (V)");
            break;

        case commandCode::STREAM_PHOTO_SPI_F:
            STREAM_PHOTO_SPI = false;
            Serial.println("Command Executed - Stopped streaming photodiode data from SPI.");
            break;

        case commandCode::STREAM_PHOTO_DIRECT_T:
            STREAM_PHOTO_DIRECT = true;
            Serial.println("Command Executed - Transmitting photodiode voltages read by Teensy ADC in real time.");
            Serial.println("PHOTO_DIR | time (ms) | photodiode voltage (V)");
            break;

        case commandCode::STREAM_PHOTO_DIRECT_F:
            STREAM_PHOTO_DIRECT = false;
            Serial.println("Command Executed - Stopped streaming photodiode data from Teensy ADC.");
            break;

        // Housekeeping
        case commandCode::TURN_HEATER_ON: 
            HEATER_ON = true;
            Serial.println("Command Executed - Heater turned ON.");
            if (!HEATER_OVERRIDE) { Serial.println("Automatic control still has priority."); }
            break;
        
        case commandCode::TURN_HEATER_OFF: 
            HEATER_ON = false;
            Serial.println("Command Executed - Heater turned OFF.");
            if (!HEATER_OVERRIDE) { Serial.println("Automatic control still has priority."); }
            break;
        
        case commandCode::HEATER_OVERRIDE_T:
            HEATER_OVERRIDE = true;
            Serial.println("Command Executed - Automatic heater control is disabled.");
            break;

        case commandCode::HEATER_OVERRIDE_F:
            HEATER_OVERRIDE = false;
            Serial.println("Command Executed - Automatic heater control is enabled.");
            break;

        case commandCode::STREAM_TEMPERATURE_T:
            STREAM_TEMPERATURE = true;
            Serial.println("Command Executed - Transmitting temperature data in real time.");
            Serial.println("time (ms) | heater status | optics temp (C) | analog temp (C) | digital temp (C)");
            break;

        case commandCode::STREAM_TEMPERATURE_F:
            STREAM_TEMPERATURE = false;
            Serial.println("Command Executed - Stopped streaming temperature data.");
            break;

        case commandCode::CALIBRATE_OPTICS_THERM:
            OPTICS_THERM_CAL_VOLTAGE = TEENSY_VOLTAGE_RES*analogRead(PIN_OPTICS_THERM);
            Serial.print("Command Executed - Optics thermistor calibrated at ");
            Serial.print(OPTICS_THERM_CAL_TEMP);
            Serial.println(" C");
            break;

        case commandCode::DISABLE_WD_RESET: 
            wdTimer.setDuration(0xFFFFFFFF); // that should do it.
            Serial.println("Command Executed - Watchdog will not be fed. Restart imminent!");
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

        case commandCode::DANGER_COMMANDS_ALLOWED_T:
            DANGER_COMMANDS_ALLOWED = true;
            Serial.println("Command Executed - !!! Potentially dangerous commands are now enabled !!!");
            break;

        case commandCode::DANGER_COMMANDS_ALLOWED_F:
            DANGER_COMMANDS_ALLOWED = false;
            Serial.println("Command Executed - Potentially dangerous commands are now disabled.");
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
        case commandCode::SCRUB_FLASH:
            scrubEvent.invoke();
            Serial.println("Command Executed - Flash scrub initiated.");
            break;

        // Fault Mitigation
        case commandCode::WIPE_EEPROM:
            wipeEEPROM();
            Serial.println("Command Executed - EEPROM has been wiped.");
            break;

        case commandCode::RESET_PERSISTENT_DATA:
            resetPersistentData();
            Serial.println("Command Executed - Persistent data cleared");
            break;
        
        case commandCode::SUPPRESS_FAULTS_T:
            SUPPRESS_FAULTS = true;
            Serial.println("Command Executed - Fault messages are suppressed. Faults will still be logged.");
            break;

        case commandCode::SUPPRESS_FAULTS_F:
            SUPPRESS_FAULTS = false;
            Serial.println("Command Executed - Fault messages enabled.");
            break;

        case commandCode::ACT_ON_FAULTS_T:
            ACT_ON_FAULTS = true;
            Serial.println("Command Executed - NS2 will attempt to correct faults");
            break;

        case commandCode::ACT_ON_FAULTS_F:
            ACT_ON_FAULTS = false;
            Serial.println("Command Executed - NS2 will NOT attempt to correct faults");
            break;

        case commandCode::SAVE_FAULTS_T:
            SAVE_FAULTS_TO_EEPROM = true;
            Serial.println("Command Executed - Faults will be saved to EEPROM");
            break;

        case commandCode::SAVE_FAULTS_F:
            SAVE_FAULTS_TO_EEPROM = false;
            Serial.println("Command Executed - Faults will NOT be saved to EEPROM");
            break;

        // Main Loop
        case commandCode::EXIT_MAIN_LOOP:
            exitMainLoopEvent.invoke();
            Serial.println("Command Executed - Exiting main loop at next opportunity.");
            break;

        // End of list
        case commandCode::SELF_DESTRUCT:
            Serial.println(">>> NANOSAM WILL SELF DESTRUCT IN 10 SECONDS <<<");
            // detonationTimer.start();
            break;

        case commandCode::DO_NOTHING: 
            Serial.println("Command Executed - Doing nothing.");
            break;
        
        default: 
            Serial.println("Code in command queue has no matching command (this should not happen).");
            Serial.println("(Command Handling)");
            break;
    }
}

/* - - - - - - printInfo - - - - - - *
 * Usage:
 *  Prints a status report of the payload
 * 
 * Inputs:
 *  None
 * Outputs:
 *  None
 */
void printInfo() {
    Serial.println("========== Status Report ==========");
    Serial.print("Uptime (ms): ");
    Serial.println(millis());
    Serial.print("Mode: ");
    switch (scienceMode.getMode()) {
        case SAFE_MODE: Serial.println("Safe");
            break;
        case STANDBY_MODE: Serial.println("Standby");
            break;
        case SUNSET_MODE: Serial.println("Sunset");
            break;
        case PRE_SUNRISE_MODE: Serial.println("Watching for Sunrise");
            break;
        case SUNRISE_MODE: Serial.println("Sunrise");
            break;
        default: Serial.println("Mode Not Recognized!");
            break;
    }
    Serial.print("Total Restarts: ");
    Serial.println(payloadData.startCount);
    Serial.print("Unexpected Restarts: ");
    Serial.println(payloadData.consecutiveBadRestarts);
    Serial.print("EEPROM Writes: ");
    Serial.println(payloadData.eepromWriteCount);
    Serial.print("Analog Board Current (A): ");
    Serial.println(latestHkSample.analogCurrent);
    Serial.print("Digital Board Current (A): ");
    Serial.println(latestHkSample.digitalCurrent);
    Serial.print("Digital Reg PG (V): ");
    Serial.println(latestHkSample.digitalRegPG);
    Serial.print("Heater Control: ");
    if (HEATER_OVERRIDE) { Serial.println("Manual"); } 
    else { Serial.println("NS2 has priority"); }
    Serial.print("Heater Status: ");
    if (HEATER_ON) { Serial.println("ON"); } 
    else { Serial.println("OFF"); }
    Serial.print("Risky Commands: ");
    if (DANGER_COMMANDS_ALLOWED) { Serial.println("Enabled"); } 
    else { Serial.println("Disabled"); }
    Serial.print("Command Execution: ");
    if (isPaused) { Serial.println("Queue"); } 
    else { Serial.println("Immediate"); }
    Serial.print("Fault Messages: ");
    if (SUPPRESS_FAULTS) { Serial.println("Disabled"); } 
    else { Serial.println("Enabled"); }
    Serial.print("Fault Correction: ");
    if (ACT_ON_FAULTS) { Serial.println("Enabled"); } 
    else { Serial.println("Disabled"); }
    Serial.print("Save Faults: ");
    if (SAVE_FAULTS_TO_EEPROM) { Serial.println("To EEPROM"); } 
    else { Serial.println("On stack only"); }
    Serial.println("========== End Report ==========");
}
