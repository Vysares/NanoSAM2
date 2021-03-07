#ifndef FAULT_MANAGER_H
#define FAULT_MANAGER_H

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries
#include <EEPROM.h>

// NS2 headers
#include "config.hpp"
#include "hammingBlock.hpp"
#include "fault.hpp"


/* - - - - - - Class Declaration - - - - - - */
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
        void clearResetCount();
};

#endif
