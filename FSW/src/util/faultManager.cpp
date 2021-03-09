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




FaultManager::FaultManager() {
    m_logIdx = 0;
    m_startCount = 0;
    m_unexpectedRestartCount = 0;
    m_eepromWriteCount = 0;
 }



void FaultManager::log(uint8_t code) {
    if (code >= faultCode::ERR_CODE) {
        Serial.println("Warning, tried to log a fault with invalid fault code (Fault Manager)");
        return;
    }
    m_faultList[m_logIdx] = Fault(code);
    m_logIdx++;
    saveEEPROM();
}



void FaultManager::saveEEPROM() {
    m_eepromWriteCount++; // one more write to EEPROM
    uint8_t rawData[m_encodedData.DECODED_MEMSIZE] = {}; // contiguous array to store unencoded data

    // copy data to rawData array
    size_t bytesCopied = 0;
    memAppend(rawData, &m_eepromWriteCount, sizeof(m_eepromWriteCount), &bytesCopied);
    memAppend(rawData, &m_startCount, sizeof(m_startCount), &bytesCopied);
    memAppend(rawData, &m_unexpectedRestartCount, sizeof(m_unexpectedRestartCount), &bytesCopied);

    // encode the data
    m_encodedData.encodeData(rawData);

    // write to EEPROM
    for (int byteNum = 0; byteNum < m_encodedData.MEMSIZE; byteNum++) {
        int eepromAddress = PERSIST_DATA_ADDR + byteNum;
        //EEPROM.write(eepromAddress, encodedData[byteNum]);
    }
}



void FaultManager::loadEEPROM() {
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