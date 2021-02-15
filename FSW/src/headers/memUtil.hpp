#ifndef MEMUTIL_H
#define MEMUTIL_H 

/* - - - - - - Includes - - - - - - */
// C++ libraries
#include <string>

// Other libraries
#include <SerialFlash.h> // for file I/O to flash modules
#include <SPI.h> // will probably use this shortly, but it is not in use yet
#include <TimeLib.h>

// NS2 headers
#include "config.hpp"

/* - - - - - - Declarations - - - - - - */
float dataProcessing();
void scienceMemoryHandling();
void updateBuffer(float sample, int &index);
bool saveBuffer(int &index);
unsigned long calcTimestamp(); // currently outputs relative timestamp instead of absolute timestamp

#endif