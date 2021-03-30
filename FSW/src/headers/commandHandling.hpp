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
bool checkIfCommandAllowed(int command);
void executeAllCommands();
void clearCommandQueue();
void executeCommand(int command);
void printInfo();

namespace commandCode {
    // command codes are wrapped in a namespace so they are not global
    enum Code { // all possible commands
        // commands are 1 indexed so that the default initializer can be used for the command queue
        // Info
        INFO = 1,                       // Prints a report of system data

        // Mode Change
        ENTER_SAFE_MODE,                // enter safemode state
        ENTER_STANDBY_MODE,             // enter standby state
        ENTER_SUNSET_MODE,              // enter sunset data collection mode
        ENTER_PRE_SUNRISE_MODE,         // enter watch-for-sunset mode
        ENTER_SUNRISE_MODE,             // enter sunrise data collection mode

        // Data Collection
        SAVE_BUFFER,                    // saves the buffer in the next available file slot
        DOWNLINK_START,                 // start downlink at next available time
        STREAM_PHOTO_SPI_T,             // start streaming photodiode data read from SPI
        STREAM_PHOTO_SPI_F,             // stop streaming photodiode data read from SPI
        STREAM_PHOTO_DIRECT_T,          // start streaming photodiode data read directly from pin
        STREAM_PHOTO_DIRECT_F,          // stop streaming photodiode data read directly from pin
        PRINT_PHOTO_SPI_SINGLE,         // print a single photodiode sample read from SPI
        PRINT_PHOTO_DIR_SINGLE,         // print a single photodiode sample read from pin
        
        // Housekeeping
        TURN_HEATER_ON,                 // turn heater on, does not override housekeeping heater control
        TURN_HEATER_OFF,                // turn heater off, does not override housekeeping heater control
        HEATER_OVERRIDE_T,              // disables housekeeping heater control
        HEATER_OVERRIDE_F,              // enables housekeeping heater control
        STREAM_TEMPERATURE_T,           // start streaming temperature measurements
        STREAM_TEMPERATURE_F,           // stop streaming temperature measurements
        CALIBRATE_OPTICS_THERM,         // take new voltage baseline at known temp for optics thermistor

        // Command Handling
        PAUSE_EXECUTE_COMMANDS,         // pause command execution
        RESUME_EXECUTE_COMMANDS,        // resume command execution
        CLEAR_COMMAND_QUEUE,            // clears all commands from queue
        DANGER_COMMANDS_ALLOWED_T,      // allow potentially dangerous commands
        DANGER_COMMANDS_ALLOWED_F,      // disallow potentially dangerous commands
        
        // ADCS
        ADCS_POINTING_AT_SUN_T,         // sets pointing at sun flag to true
        ADCS_POINTING_AT_SUN_F,         // sets pointing at sun flag to false

        // Memory
        SCRUB_FLASH,                    // start scrubbing the flash memory for errors

        // Fault Mitigation
        WIPE_EEPROM,                   // completely wipes the EEPROM and then resets persistent data
        RESET_PERSISTENT_DATA,         // resets all persistent data to default values
        SUPPRESS_FAULTS_T,             // disables new fault messages, faults are still logged
        SUPPRESS_FAULTS_F,             // allows new fault messages
        ACT_ON_FAULTS_T,               // enables corrective action when new faults are detected
        ACT_ON_FAULTS_F,               // disables corrective action when new faults are detected
        SAVE_FAULTS_T,                 // enables saving of new faults to EEPROM
        SAVE_FAULTS_F,                 // disables saving of new faults to EEPROM
        DISABLE_WD_RESET,              // disable watchdog reset signal, forcing a restart

        // System Commands
        EXIT_MAIN_LOOP,                // exits the main loop

        // End of list
        SELF_DESTRUCT,                
        DO_NOTHING                    // do nothing. KEEP THIS LAST IN THE ENUM, it is used for indexing.
    };
};

#endif
