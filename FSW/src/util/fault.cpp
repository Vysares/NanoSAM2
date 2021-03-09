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


Fault::Fault(uint8_t code) {
    m_code = code;
    m_timestamp = millis();

    // encode fault data in a single Hamming block
    uint8_t faultData[MESSAGE_SIZE] = {};
    memcpy(faultData, &m_code, sizeof(m_code));
    memcpy(faultData + sizeof(m_code), &m_timestamp, sizeof(m_timestamp));
    encodedBlock.encodeMessage(faultData);
}


Fault::Fault() {
    m_code = faultCode::ERR_CODE;
    m_timestamp = 0;
    m_occurences = 0;

}
