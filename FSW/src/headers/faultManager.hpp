#ifndef FAULT_MANAGER_H
#define FAULT_MANAGER_H

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries
#include <EEPROM.h>

// NS2 headers
#include "config.hpp"

/* - - - - - - Struct Declarations - - - - - - */

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

        // Bad News
        EEPROM_CORRUPTED,       // EEPROM data is corrupted beyond rescue
        ERR_CODE
    };
    const int COUNT = ERR_CODE + 1;
};

struct FaultReport {
    uint8_t occurrences = 0;
    uint16_t startNum = 1;
    uint32_t timestamp = 0;
    bool pendingAction = 0; // not written to EEPROM 
    static const int MEMSIZE = sizeof(occurrences) + sizeof(startNum) + sizeof(timestamp);
};

struct PayloadData {
        uint32_t eepromWriteCount = 0;
        uint8_t expectingRestartFlag = 0;
        uint16_t startCount = 1;
        uint16_t consecutiveBadRestarts = 0;
        uint8_t recoveredMode = 0;
        static const size_t MEMSIZE = 10;
};

extern PayloadData payloadData;

/* - - - - - - Function Declarations - - - - - - */
void logFault(int code);
void feedWD();
void handleFaults();

void wipeEEPROM();
void resetPersistentData();

void recordNewStart();
void prepareForRestart();
void saveEEPROM();
void loadEEPROM();
int seekEEPROM();
void resetFaultCounts();

#endif
