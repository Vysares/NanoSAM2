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
};

class Fault {
    private:
        uint8_t m_code; // single byte int to store fault code
        unsigned long m_timestamp;
        unsigned int m_rootAddr;
        HammingBlock encodedBlock;

    public:
        Fault();
        Fault(uint8_t code);
        unsigned int getCode() { return m_code; }
        void resolve(); // remove self from list
};

#endif
