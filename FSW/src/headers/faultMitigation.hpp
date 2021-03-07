#ifndef FAULT_MITI_H
#define FAULT_MITI_H

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries
#include <EEPROM.h>

// NS2 headers
#include "config.hpp"
#include "hammingBlock.hpp"

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
        void writeToEEPROM();
        void resolve(); // remove self from list
};
        

class FaultManager {
    private:
        Fault m_faultList[MAX_FAULTS];
        uint8_t m_encodedData[FAULT_LOG_MEMSIZE];
        unsigned int m_nextRootAddress;
    public:
        FaultManager();
        void log(uint8_t code);
        void saveEEPROM();
        void loadEEPROM();
        void clearLog();
        
};




/* - - - - - - Declarations - - - - - - */
void clearResetCount();

#endif
