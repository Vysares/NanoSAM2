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


void clearResetCount() {
    EEPROM.write(RESET_COUNT_ADDR, 0);
}

Fault::Fault(uint8_t code) {
    m_code = code;
    m_timestamp = millis();

    // encode fault data in a single Hamming block
    uint8_t faultData[MESSAGE_SIZE] = {};
    memcpy(faultData, &m_code, sizeof(m_code));
    memcpy(faultData + sizeof(m_code), &m_timestamp, sizeof(m_timestamp));
    encodedBlock.encodeMessage(faultData);
}


FaultManager::FaultManager() { }

void FaultManager::log(uint8_t code) {
    if (code >= faultCode::ERR_CODE) {
        Serial.println("Warning, tried to log a fault with invalid fault code (Fault Manager)");
        return;
    }
    
    m_faultList[0] = Fault(code);
}

