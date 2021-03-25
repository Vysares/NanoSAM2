/* faultManager.cpp handles NS2 payload fault mitigation
 * Usage:
 *  module functionality
 *  function definitions
 *  put function declarations in memUtil.hpp
 * 
 * Modules encompassed:
 *  Fault Detection
 *  Fault Logging
 *  Fault Mitigation
 *
 * Additional files needed for compilation:
 *  config.hpp
 *  encodedFile.hpp
 *  
 */

/* - - - - - - Includes - - - - - - */
// All libraries are put in dataCollection.hpp
// NS2 headers
#include "../headers/faultManager.hpp"
#include "../headers/encodedFile.hpp"
#include "../headers/commandHandling.hpp"
#include "../headers/housekeeping.hpp"

/* Module Variable Definitions */
// fault log
PayloadData payloadData;
static FaultReport faultLog[faultCode::COUNT];

// decoded size of data on EEPROM, in terms of bytes
const size_t EEPROM_DECODED_SIZE = faultCode::COUNT * FaultReport::MEMSIZE + PayloadData::MEMSIZE; 

// flag to indicate whether any faults were logged in the current main loop iteration
static bool detectedFault = false; 

/* - - - - - - Module Driver Functions - - - - - - */

/* - - - - - - logFault - - - - - - *
 * Usage:
 *  records a new occurrence of a fault
 *  
 * Inputs:
 *  code - fault code to log
 * 
 * Outputs:
 *  None
 */
void logFault(int code) {
    if (SUPPRESS_FAULTS) { return; }
    detectedFault = true;

    // check if fault is valid
    if (code >= faultCode::ERR_CODE) {
        Serial.print("Warning, tried to log a fault with invalid fault code: ");
        Serial.println(code);
        return;
    }

    Serial.print("Fault Logged: ");
    Serial.println(code);

    // update corresponding fault report
    faultLog[code].startNum = payloadData.startCount;
    faultLog[code].timestamp = millis();

    if (faultLog[code].occurrences < 255) {
        faultLog[code].occurrences++;
        
        // mark for action on first and max occurrences
        if (faultLog[code].occurrences == 1 || faultLog[code].occurrences == 255) {
            faultLog[code].pendingAction = 1;
        }
    }
}

/* - - - - - - feedWD - - - - - - *
 * Usage:
 *  resets the watchdog
 *  
 * Inputs:
 *  None
 * Outputs:
 *  None
 */
void feedWD() {
    // TODO: some checks here to make sure everything is okay before feeding the dog?
    digitalWrite(PIN_WD_RESET, HIGH);
    delayMicroseconds(WD_PULSE_DUR_MICROSEC); // we may not even need this, since digitalWrite takes some time to execute.
    digitalWrite(PIN_WD_RESET, LOW);
}
                                                                     
/* - - - - - - handleFaults - - - - - - *
 * Usage:
 *  takes corrective action for all logged faults
 *  
 * Inputs:
 *  None
 * Outputs:
 *  None
 */
void handleFaults() {
    if (!detectedFault || !ACT_ON_NEW_FAULTS) { return; }
    detectedFault = false;

    for (int code = 0; code < faultCode::COUNT; code++) { // for each fault report...
        
        if (faultLog[code].pendingAction == 1) {
            faultLog[code].pendingAction = 0;

            /* Take action to correct fault */
            // TODO: Expand to include more faults
            switch (code) {
                case faultCode::UNEXPECTED_RESTART:
                    Serial.println("Unexpected restart detected. Entering safe mode.");
                    executeCommand(commandCode::ENTER_SAFE_MODE);
                    break;
                
                // temp too hot
                case faultCode::ANALOG_TOO_HOT:
                case faultCode::DIGITAL_TOO_HOT:
                case faultCode::OPTICS_TOO_HOT:
                    Serial.println("Warning - one or more temperatures above acceptable range!");
                    executeCommand(commandCode::TURN_HEATER_OFF);
                    break;

                // tempt too cold
                case faultCode::ANALOG_TOO_COLD:
                case faultCode::DIGITAL_TOO_COLD:
                case faultCode::OPTICS_TOO_COLD:
                    Serial.println("Warning - one or more temperatures below acceptable range!");
                    executeCommand(commandCode::TURN_HEATER_ON);
                    break;

                // EEPROM corrupted
                case faultCode::EEPROM_CORRUPTED:
                    Serial.println("Detected corrupted data in EEPROM, entering safe mode.");
                    executeCommand(commandCode::ENTER_SAFE_MODE);
                    break;

                // if fault has no corrective action
                default:
                    Serial.print("Fault code ");
                    Serial.print(code);
                    Serial.println(" has no assigned corrective action.");
                    break;
            }
        }
    }
}

/* - - - - - - wipeEEPROM - - - - - - *
 * Usage:
 *  Clears every byte in EEPROM and resets all persistent data to default values.
 *  Are you sure you want to do that?
 *  As a safeguard, this must be called twice before EEPROM is cleared.
 *  
 * Inputs:
 *  None
 * Outputs:
 *  None
 */
void wipeEEPROM() {
    Serial.print("The current EEPROM write count is ");
    Serial.println(payloadData.eepromWriteCount);

    // Completely wipe the EEPROM
    for (int i = 0; i < EEPROM_SIZE; i++) {
        EEPROM.write(i, 0);
    }
    payloadData.eepromWriteCount = 1;
    resetPersistentData();
}

/* - - - - - - resetPersistentData - - - - - - *
 * Usage:
 *  Resets all persistent data to default values.
 *  Does not change EEPROM write counter.
 *  
 * Inputs:
 *  None
 * Outputs:
 *  None
 */
void resetPersistentData() {
    payloadData.expectingRestartFlag = 0;
    payloadData.startCount = 1;
    payloadData.consecutiveBadRestarts = 0;
    payloadData.recoveredMode = 0;

    for (int i = 0; i < faultCode::COUNT; i++) {
        faultLog[i].occurrences = 0;
        faultLog[i].startNum = 1;
        faultLog[i].timestamp = 0;
    }
    saveEEPROM();
}

/* - - - - - - recordNewStart - - - - - - *
 * Usage:
 *  Increments start count, call first thing in init()
 *  
 * Inputs:
 *  None
 * Outputs:
 *  None
 */
void recordNewStart() {
    payloadData.startCount++; // increment total start count
    
    // check if restart was unexpected
    if (payloadData.expectingRestartFlag != EXPECTING_RESTART_FLAG) {
        logFault(faultCode::UNEXPECTED_RESTART);
        payloadData.consecutiveBadRestarts++;
    } else { // restart was expected
        payloadData.consecutiveBadRestarts = 0;
    }
    payloadData.expectingRestartFlag = 0; // reset the flag
    saveEEPROM();
}

/* - - - - - - prepareForRestart - - - - - - *
 * Usage:
 *  Sets restart flag to indicate restart was expected 
 *   and saves the current science mode to EEPROM
 *  
 * Inputs:
 *  None
 * Outputs:
 *  None
 */
void prepareForRestart() {
    payloadData.expectingRestartFlag = EXPECTING_RESTART_FLAG;
    payloadData.recoveredMode = scienceMode.getMode();
    saveEEPROM();
}

/* - - - - - - saveEEPROM - - - - - - *
 * Usage:
 *  Saves payload data and fault log to EEPROM.
 *  Does not stress the EEPROM, since only changed bytes are overwritten. 
 *  
 * Inputs:
 *  None
 * 
 * Outputs:
 *  The total number of bytes overwritten
 */
int saveEEPROM() {

    payloadData.eepromWriteCount++; // one more write to EEPROM
    int bytesWritten = 0;
    uint8_t rawData[EEPROM_DECODED_SIZE] = {}; // contiguous array to store unencoded data

    // copy persistent data to rawData array
    size_t bytesCopied = 0;
    memAppend(rawData, &payloadData.eepromWriteCount, sizeof(payloadData.eepromWriteCount), &bytesCopied);
    memAppend(rawData, &payloadData.startCount, sizeof(payloadData.startCount), &bytesCopied);
    memAppend(rawData, &payloadData.consecutiveBadRestarts, sizeof(payloadData.consecutiveBadRestarts), &bytesCopied);
    memAppend(rawData, &payloadData.recoveredMode, sizeof(payloadData.recoveredMode), &bytesCopied);

    // copy fault data to rawData array
    for (int i = 0; i < faultCode::COUNT; i++) {
        memAppend(rawData, &faultLog[i].occurrences, sizeof(faultLog[i].occurrences), &bytesCopied);
        memAppend(rawData, &faultLog[i].startNum, sizeof(faultLog[i].startNum), &bytesCopied);
        memAppend(rawData, &faultLog[i].timestamp, sizeof(faultLog[i].timestamp), &bytesCopied);
    }

    // encode the data
    EncodedFile<EEPROM_DECODED_SIZE> encodedData = EncodedFile<EEPROM_DECODED_SIZE>();
    encodedData.encodeData(rawData);

    // write to EEPROM
    for (int byteNum = 0; byteNum < encodedData.MEMSIZE; byteNum++) {
        int eepromAddress = PERSIST_DATA_ADDR + byteNum;
        uint8_t dataToWrite = encodedData.getData()[byteNum];

        if (EEPROM.read(eepromAddress) != dataToWrite) { // only overwrite if the byte has changed. This extends the life of the EEPROM.
            EEPROM.write(eepromAddress, dataToWrite);
            bytesWritten++;
        }
    }
    return bytesWritten;
}

/* - - - - - - loadEEPROM - - - - - - *
 * Usage:
 *  updates payload data and fault log from EEPROM
 *  
 * Inputs:
 *  None
 * Outputs:
 *  None
 */
void loadEEPROM() { 
    // like saveEEPROM, but in reverse

    EncodedFile<EEPROM_DECODED_SIZE> encodedData = EncodedFile<EEPROM_DECODED_SIZE>();
    uint8_t eepromData[encodedData.MEMSIZE] = {};

    // read EEPROM
    for (int byteNum = 0; byteNum < encodedData.MEMSIZE; byteNum++) {
        int eepromAddress = PERSIST_DATA_ADDR + byteNum;
        eepromData[byteNum] = EEPROM.read(eepromAddress);
    }

    // decode and scrub EEPROM data
    encodedData.fill(eepromData);
    ScrubReport scrubInfo = encodedData.scrub();

    if (scrubInfo.uncorrected != 0) { // oh no!
        logFault(faultCode::EEPROM_CORRUPTED);
    }

    // extract system data
    size_t bytesCopied = 0;
    memExtract(encodedData.getDecodedData(), &payloadData.eepromWriteCount, sizeof(payloadData.eepromWriteCount), &bytesCopied);
    memExtract(encodedData.getDecodedData(), &payloadData.startCount, sizeof(payloadData.startCount), &bytesCopied);
    memExtract(encodedData.getDecodedData(), &payloadData.consecutiveBadRestarts, sizeof(payloadData.consecutiveBadRestarts), &bytesCopied);
    memExtract(encodedData.getDecodedData(), &payloadData.recoveredMode, sizeof(payloadData.recoveredMode), &bytesCopied);

    // copy fault data to fault log
    for (int i = 0; i < faultCode::COUNT; i++) {
        memExtract(encodedData.getDecodedData(), &faultLog[i].occurrences, sizeof(faultLog[i].occurrences), &bytesCopied);
        memExtract(encodedData.getDecodedData(), &faultLog[i].startNum, sizeof(faultLog[i].startNum), &bytesCopied);
        memExtract(encodedData.getDecodedData(), &faultLog[i].timestamp, sizeof(faultLog[i].timestamp), &bytesCopied);
    }
}

/* - - - - - - resetFaultCounts - - - - - - *
 * Usage:
 *  Resets the occurrence count of each fault report to 0.
 *  timestamp and mode of last fault occurrence are not changed
 *  
 * Inputs:
 *  None
 * Outputs:
 *  None
 */
void resetFaultCounts() {
    for (int i = 0; i < faultCode::COUNT; i++) {
        faultLog[i].occurrences = 0;
        faultLog[i].pendingAction = 0;
    }
    saveEEPROM();
}

