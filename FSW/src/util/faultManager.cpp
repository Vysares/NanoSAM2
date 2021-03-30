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
#include "../headers/timing.hpp"

/* Module Variable Definitions */
// fault log
PayloadData payloadData;
static FaultReport faultLog[faultCode::COUNT];

// decoded size of data on EEPROM, in terms of bytes
const size_t EEPROM_DECODED_SIZE = faultCode::COUNT * FaultReport::MEMSIZE + PayloadData::MEMSIZE; 
const size_t MAX_ADDRESS = EncodedFile<EEPROM_DECODED_SIZE>::MEMSIZE * (EEPROM_SIZE / EncodedFile<EEPROM_DECODED_SIZE>::MEMSIZE);

// flag indicating if save to EEPROM is required
bool saveRequired = false;

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
    // check if fault is valid
    if (code >= faultCode::ERR_CODE) {
        Serial.print("Warning, tried to log a fault with invalid fault code: ");
        Serial.println(code);
        return;
    }

    // update corresponding fault report
    faultLog[code].startNum = payloadData.startCount;
    faultLog[code].timestamp = millis();

    if (faultLog[code].occurrences < 255) { 
        faultLog[code].occurrences++; 
        saveRequired = true;
    }
    faultLog[code].pendingAction = true;
    
    // Print message
    if (!SUPPRESS_FAULTS) {
        Serial.print("Fault Logged: ");
        Serial.print(code);
        Serial.print(" (");
        Serial.print(faultLog[code].occurrences);
        Serial.println(")");
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
    if (SAVE_FAULTS_TO_EEPROM && saveRequired) { 
        saveEEPROM();
        saveRequired = false;    
    }

    for (int code = 0; code < faultCode::COUNT; code++) { // for each fault report...

        // check if fault needs action
        // note that the action flag is set to false even if ACT_ON_FAULTS is false. 
        //  this prevents a buildup of multiple faults that may conflict. (e.g. the optics temp cannot be both too hot and too cold)
        if (faultLog[code].pendingAction) { faultLog[code].pendingAction = false; }  
        else { continue; }
        
        // skip all if corrective actions are disabled
        if (!ACT_ON_FAULTS) { continue; }
        
        /* Take action to correct fault */
        // TODO: Expand to include more faults
        switch (code) {

            case faultCode::UNEXPECTED_RESTART:
                scienceMode.setMode(SAFE_MODE);
                Serial.println("Corrective Action Taken - Entering Safe Mode.");
                Serial.println("(Unexpected restart!)");
                break;

            // temp too hot
            case faultCode::ANALOG_TOO_HOT:
            case faultCode::DIGITAL_TOO_HOT:
            case faultCode::OPTICS_TOO_HOT:

            // temp too cold
            case faultCode::ANALOG_TOO_COLD:
            case faultCode::DIGITAL_TOO_COLD:
            case faultCode::OPTICS_TOO_COLD:
                if (HEATER_OVERRIDE) {
                    HEATER_OVERRIDE = false;
                    Serial.println("Corrective Action Taken - Automatic heater control re-enabled.");
                    Serial.println("(One or more temperatures out of acceptable range!)");
                }
                break;

            // EEPROM corrupted
            case faultCode::EEPROM_CORRUPTED:
                if (scienceMode.getMode() != SAFE_MODE) {
                    Serial.println("Corrective Action Taken - Mode set to safemode.");
                    Serial.println("(EEPROM corrupted!)");
                    scienceMode.setMode(SAFE_MODE);
                }
                break;

            // if fault has no corrective action
            default:
                // Serial.print("Fault code ");
                // Serial.print(code);
                // Serial.println(" has no assigned corrective action.");
                break;
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
        faultLog[i].pendingAction = 0;
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

    // shift EEPROM address
    int startAddress = (payloadData.eepromWriteCount * encodedData.MEMSIZE) % MAX_ADDRESS;
    Serial.println(startAddress);

    // write to EEPROM
    for (int byteNum = 0; byteNum < encodedData.MEMSIZE; byteNum++) {
        int eepromAddress = startAddress + byteNum;
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

    // read data from EEPROM
    int startAddress = seekEEPROM(); // find latest EEPROM block
    Serial.println(startAddress);
    for (int byteNum = 0; byteNum < encodedData.MEMSIZE; byteNum++) {
        eepromData[byteNum] = EEPROM.read(startAddress + byteNum);
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

/* - - - - - - seekEEPROM - - - - - - *
 * Usage:
 *  Finds the address in EEPROM where persistent data was last saved
 *  
 * Inputs:
 *  None

 * Outputs:
 *  Start address in EEPROM where data was most recently written
 */
int seekEEPROM() {

    EncodedFile<EEPROM_DECODED_SIZE> encodedData = EncodedFile<EEPROM_DECODED_SIZE>();
    uint8_t eepromData[encodedData.MEMSIZE] = {};

    // seek the latest EEPROM address
    uint32_t highestWriteCount = 0;
    uint32_t writeCount = 0;
    for (int address = 0; address < MAX_ADDRESS; address += encodedData.MEMSIZE) { // for each EEPROM block...

        // read data from EEPROM
        for (int byteNum = 0; byteNum < encodedData.MEMSIZE; byteNum++) {
            eepromData[byteNum] = EEPROM.read(address + byteNum);
        }
        encodedData.fill(eepromData);
        encodedData.scrub();
        memcpy(&writeCount, encodedData.getDecodedData(), sizeof(writeCount));
        
        // compare write countst o find the highest. Higher write count corresponds to more recent write
        if (writeCount >= highestWriteCount) {
            highestWriteCount = writeCount;
        }
    }
    return (highestWriteCount * encodedData.MEMSIZE) % MAX_ADDRESS;
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
        faultLog[i].pendingAction = false;
    }
    saveEEPROM();
}