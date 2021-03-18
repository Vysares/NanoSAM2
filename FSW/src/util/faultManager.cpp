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
static FaultReport faultLog[faultCode::COUNT];
// decoded size of data on EEPROM, in terms of bytes
const size_t EEPROM_DECODED_SIZE = faultCode::COUNT * FaultReport::MEMSIZE + PayloadData::MEMSIZE; 
// flag to indicate whether any faults were logged in the current main loop iteration
static bool detectedFault = false; 

/* - - - - - - Module Driver Functions - - - - - - */

/* - - - - - - logFault - - - - - - *
 * Usage:
 *  records a new occurence of a fault
 *  
 * Inputs:
 *  code - fault code to log
 * 
 * Outputs:
 *  None
 */
void logFault(uint8_t code) {
    detectedFault = true;

    // check if fault is valid
    if (code >= faultCode::ERR_CODE) {
        Serial.println("Warning, tried to log a fault with invalid fault code (Fault Manager)");
        return;
    }

    Serial.print("Fault Logged: ");
    Serial.println(code);

    // update corresponding fault report
    faultLog[code].startNum = payloadData.startCount;
    faultLog[code].timestamp = millis();

    if (faultLog[code].occurences < 255) {
        faultLog[code].occurences++;
        
        // mark for action on first and max occurences
        if (faultLog[code].occurences == 1 || faultLog[code].occurences == 255) {
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
    saveEEPROM();

    for (int code = 0; code < faultCode::COUNT; code++) { // for each fault report...
        
        if (faultLog[code].pendingAction == 1) {
            faultLog[code].pendingAction = 0;

            /* Take action to correct fault */
            // TODO: Expand to include more faults
            switch (code) {
                case faultCode::UNEXPECTED_RESTART:
                    executeCommand(commandCode::ENTER_SAFE_MODE);
                    break;
                
                // temp too hot
                case faultCode::ANALOG_TOO_HOT:
                case faultCode::DIGITAL_TOO_HOT:
                case faultCode::OPTICS_TOO_HOT:
                    executeCommand(commandCode::FORCE_HEATER_ON_F);
                    break;

                // tempt too cold
                case faultCode::ANALOG_TOO_COLD:
                case faultCode::DIGITAL_TOO_COLD:
                case faultCode::OPTICS_TOO_COLD:
                    executeCommand(commandCode::FORCE_HEATER_ON_T);
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

/* - - - - - - clearAllPersistentData - - - - - - *
 * Usage:
 *  Clears all persistent data
 *  
 * Inputs:
 *  None
 * Outputs:
 *  None
 */
void clearAllPersistentData() {
    payloadData.expectingRestartFlag = 0;
    payloadData.startCount = 1;
    payloadData.consecutiveBadRestarts = 0;
    payloadData.recoveredMode = 0;

    for (int i = 0; i < faultCode::COUNT; i++) {
        faultLog[i].occurences = 0;
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

/* - - - - - - Helper Functions - - - - - - */

/* - - - - - - saveEEPROM - - - - - - *
 * Usage:
 *  Saves payload data and fault log to EEPROM.
 *  Does not stress the EEPROM, since only changed bytes are overwritten. 
 *  
 * Inputs:
 *  None
 * Outputs:
 *  None
 */
void saveEEPROM() {

    payloadData.eepromWriteCount++; // one more write to EEPROM
    uint8_t rawData[EEPROM_DECODED_SIZE] = {}; // contiguous array to store unencoded data

    // copy persistent data to rawData array
    size_t bytesCopied = 0;
    memAppend(rawData, &payloadData.eepromWriteCount, sizeof(payloadData.eepromWriteCount), &bytesCopied);
    memAppend(rawData, &payloadData.startCount, sizeof(payloadData.startCount), &bytesCopied);
    memAppend(rawData, &payloadData.consecutiveBadRestarts, sizeof(payloadData.consecutiveBadRestarts), &bytesCopied);
    memAppend(rawData, &payloadData.recoveredMode, sizeof(payloadData.recoveredMode), &bytesCopied);

    // copy fault data to rawData array
    for (int i = 0; i < faultCode::COUNT; i++) {
        memAppend(rawData, &faultLog[i].occurences, sizeof(faultLog[i].occurences), &bytesCopied);
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
        }
    }
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

    // decode the EEPROM data
    encodedData.fill(eepromData);

    // extract system data 
    size_t bytesCopied = 0;
    memExtract(encodedData.getDecodedData(), &payloadData.eepromWriteCount, sizeof(payloadData.eepromWriteCount), &bytesCopied);
    memExtract(encodedData.getDecodedData(), &payloadData.startCount, sizeof(payloadData.startCount), &bytesCopied);
    memExtract(encodedData.getDecodedData(), &payloadData.consecutiveBadRestarts, sizeof(payloadData.consecutiveBadRestarts), &bytesCopied);
    memExtract(encodedData.getDecodedData(), &payloadData.recoveredMode, sizeof(payloadData.recoveredMode), &bytesCopied);

    // copy fault data to fault log
    for (int i = 0; i < faultCode::COUNT; i++) {
        memExtract(encodedData.getDecodedData(), &faultLog[i].occurences, sizeof(faultLog[i].occurences), &bytesCopied);
        memExtract(encodedData.getDecodedData(), &faultLog[i].startNum, sizeof(faultLog[i].startNum), &bytesCopied);
        memExtract(encodedData.getDecodedData(), &faultLog[i].timestamp, sizeof(faultLog[i].timestamp), &bytesCopied);
    }
}

/* - - - - - - resetFaultCounts - - - - - - *
 * Usage:
 *  Resets the occurence count of each fault report to 0.
 *  timestamp and mode of last fault occurence are not altered
 *  
 * Inputs:
 *  None
 * Outputs:
 *  None
 */
void resetFaultCounts() {
    for (int i = 0; i < faultCode::COUNT; i++) {
        faultLog[i].occurences = 0;
        faultLog[i].pendingAction = 0;
    }
    saveEEPROM();
}

