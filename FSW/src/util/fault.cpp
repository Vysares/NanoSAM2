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


Fault::Fault() {
    m_code = faultCode::ERR_CODE;
    m_timestamp = 0;
    m_occurences = 0;
    m_rootAddr = 0;
}