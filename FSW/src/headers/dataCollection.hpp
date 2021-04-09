#ifndef DATA_H
#define DATA_H 

/* - - - - - - Includes - - - - - - */
// C++ libraries
#include <string>

// Other libraries
#include <SerialFlash.h> // for file I/O to flash modules
#include <SPI.h> 
#include <TimeLib.h>

// NS2 headers
#include "config.hpp"

/* - - - - - - Declarations - - - - - - */
uint16_t dataProcessing();
void scienceMemoryHandling();
void updateBuffer(uint16_t sample, int &index);
bool saveBuffer();
unsigned long calcTimestamp(); // currently outputs relative timestamp instead of absolute timestamp
void downlink();
void scrubFlash();

#endif