/* faultManager.cpp defines the faultManager class, which handles NS2 payload fault mitigation
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
 *  
 */

/* - - - - - - Includes - - - - - - */
// All libraries are put in dataCollection.hpp
// NS2 headers
#include "../headers/faultManager.hpp"
#include "../headers/encodedFile.hpp"

/* Module Variable Definitions */
FaultReport faultLog[faultCode::COUNT];

void logFault(uint8_t code) {
    if (code >= faultCode::ERR_CODE) {
        Serial.println("Warning, tried to log a fault with invalid fault code (Fault Manager)");
        return;
    }
    Serial.print("Fault Logged: ");
    Serial.println(code);

    faultLog[code].occurences++;
    faultLog[code].startNum = persistentData.startCount;
    faultLog[code].timestamp = millis();

    saveEEPROM();
}


void feedWD() {
    digitalWrite(PIN_WD_RESET, HIGH);
    delayMicroseconds(WD_PULSE_DUR_MICROSEC); // we may not even need this, since digitalWrite takes some time to execute.
    digitalWrite(PIN_WD_RESET, LOW);
}
                                                                     

void handleFaults() {

}

void clearFaultLog() {
    for (int i = 0; i < faultCode::COUNT; i++) {
        faultLog[i].occurences = 0;
        faultLog[i].startNum = 1;
        faultLog[i].timestamp = 0;
    }
}

void logStart() {
    persistentData.startCount++;
    
}

void clearResetCount() {
    persistentData.unexpectedRestartCount = 0;
    saveEEPROM();
}

void saveEEPROM() {
    persistentData.eepromWriteCount++; // one more write to EEPROM
    uint8_t rawData[DECODED_MEMSIZE] = {}; // contiguous array to store unencoded data

    // copy data to rawData array
    size_t bytesCopied = 0;
    memAppend(rawData, &persisentData.eepromWriteCount, sizeof(persisentData.eepromWriteCount), &bytesCopied);
    memAppend(rawData, &persisentData.startCount, sizeof(persisentData.startCount), &bytesCopied);
    memAppend(rawData, &persisentData.unexpectedRestartCount, sizeof(persisentData.unexpectedRestartCount), &bytesCopied);

    // encode the data
    EncodedFile<
    encodedData.encodeData(rawData);

    // write to EEPROM
    for (int byteNum = 0; byteNum < m_encodedData.MEMSIZE; byteNum++) {
        int eepromAddress = PERSIST_DATA_ADDR + byteNum;
        EEPROM.write(eepromAddress, encodedData[byteNum]);
    }
}


void loadEEPROM() {
    uint8_t eepromData[PERSIST_DATA_MEMSIZE] = {}; // array to store EEPROM contents

    // read EEPROM
    for (int byteNum = 0; byteNum < m_encodedData.MEMSIZE; byteNum++) {
        int eepromAddress = PERSIST_DATA_ADDR + byteNum;
        //eepromData[byteNum] = EPROM.read(eepromAddress);
    }

    // decode the EEPROM data
    m_encodedData.fill(eepromData);
    uint8_t *decodedData = m_encodedData.decode();

    // extract system data 
    size_t bytesCopied = 0;
    memExtract(&m_eepromWriteCount, decodedData, sizeof(m_eepromWriteCount), &bytesCopied);
    memExtract(&m_startCount, decodedData, sizeof(m_startCount), &bytesCopied);
    memExtract(&m_unexpectedRestartCount, decodedData, sizeof(m_unexpectedRestartCount), &bytesCopied);
}

