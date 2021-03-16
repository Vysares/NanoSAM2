#ifndef FAULT_H
#define FAULT_H

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries

// NS2 headers
#include "config.hpp"
#include "hammingBlock.hpp"


/* - - - - - - Class Declaration - - - - - - */

namespace faultCode { 
    // fault codes are wrapped in a namespace so they are not global
    enum Code {
        UNEXPECTED_RESTART,
        FAULT_LOG_OVERFLOW,     // probably a bad sign.
        

        // Temperature
        ANALOG_TOO_HOT,
        ANALOG_TOO_COLD,
        DIGITAL_TOO_HOT,
        DIGITAL_TOO_COLD,
        OPTICS_TOO_HOT,
        OPTICS_TOO_COLD,
        
        ERR_CODE
    };
    const size_t COUNT = ERR_CODE + 1;
};

class FaultReport {
    private:
        uint8_t m_code; // single byte int to store fault code
        uint8_t m_occurences;
        uint16_t m_startNum;
        uint32_t m_timestamp;
        bool m_active;

    public:
        FaultReport();
        FaultReport(uint8_t code, uint16_t startNum);

        unsigned int getCode() { return m_code; }
        void resolve(); // remove self from list
        bool isActive() { return m_active; }
        uint8_t *getData();
        void clear();

};

#endif
