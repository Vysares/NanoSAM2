#ifndef FAULT_MANAGER_H
#define FAULT_MANAGER_H

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries

// NS2 headers
#include "config.hpp"
#include "fault.hpp"


/* - - - - - - Class Declaration - - - - - - */
class FaultManager {
    protected:
        static const int FAULT_LOG_MEMSIZE = MAX_FAULTS * HAMMING_BLOCK_SIZE;
        int m_logIdx;

        // fault log
        Fault m_faultList[MAX_FAULTS];

        // system data
        EncodedFile<PERSIST_DATA_MEMSIZE> m_encodedData;
        uint8_t m_recoveredMode;
        uint16_t m_startCount;
        uint16_t m_unexpectedRestartCount;
        uint32_t m_eepromWriteCount;

    public:
        FaultManager();

        void log(uint8_t code);
        

        void saveEEPROM();
        void loadEEPROM();
        void clearLog();
        void clearResetCount();
};

#endif
