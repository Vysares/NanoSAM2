#ifndef FAULT_MANAGER_H
#define FAULT_MANAGER_H

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries

// NS2 headers
#include "config.hpp"
#include "fault.hpp"

/* - - - - - - Declarations - - - - - - */
namespace faultCode { 
    // fault codes are wrapped in a namespace so they are not global
    enum Code {
        UNEXPECTED_RESTART,

        // Temperature
        ANALOG_TOO_HOT,
        ANALOG_TOO_COLD,
        DIGITAL_TOO_HOT,
        DIGITAL_TOO_COLD,
        OPTICS_TOO_HOT,
        OPTICS_TOO_COLD,

        // Power
        DREG_OUT_OF_RANGE,
        
        ERR_CODE
    };
    const size_t COUNT = ERR_CODE + 1;
};

struct FaultReport {
    uint8_t occurences = 0;
    uint8_t pendingAction = 0; 
    uint16_t startNum = 1;
    uint32_t timestamp = 0;
    static const size_t MEMSIZE = 64;
};

struct PayloadData {
        uint8_t expectingRestartFlag = 0;
        uint16_t startCount = 1;
        uint16_t consecutiveBadRestarts = 0;
        uint32_t eepromWriteCount = 0;
        uint8_t recoveredMode = 0;
        static const size_t MEMSIZE = 72;
};

PayloadData payloadData;


void logFault(int code);
void feedWD();
void handleFaults();

void clearAllPersistentData();
void recordNewStart();
void prepareForRestart();
void saveEEPROM();
void loadEEPROM();
void resetFaultCounts();

#endif
