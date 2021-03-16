/* fault.cpp defines the Fault class
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
#include "../headers/fault.hpp"


FaultReport::FaultReport() {
    m_code = faultCode::ERR_CODE;
    m_timestamp = 0;
    m_startNum = 1;
    m_occurences = 0;
    m_active = false;
}


FaultReport::FaultReport(uint8_t coden uint16_t startNum) : FaultReport::FaultReport() {
    m_code = code;
    m_timestamp = millis();
    m_startNum = startNum;
    

    // encode fault data in a single Hamming block
    
}



uint8_t *FaultReport::getData() {
    static uint8_t faultData[MESSAGE_SIZE] = {};
    int bytesCopied = 0;
    memAppend(faultData, &m_code, sizeof(m_code), &bytesCopied);
    memAppend(faultData, &m_occurences, sizeof(m_occurences), &bytesCopied);
    memAppend(faultData, &m_startNum, sizeof(m_startNum), &bytesCopied);
    memAppend(faultData, &m_timestamp, sizeof(m_timestamp), &bytesCopied);

    return faultData;
}

void FaultReport::clear() {

}