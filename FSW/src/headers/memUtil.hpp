#ifndef MEMUTIL_H
#define MEMUTIL_H 

/* - - - - - - Includes - - - - - - */
// C++ libraries
#include <string>

// Other libraries
#include <SerialFlash.h> // for file I/O to flash modules
#include <SPI.h> // will probably use this shortly, but it is not in use yet

// NS2 headers


/* - - - - - - Declarations - - - - - - */
float dataProcessing();
void scienceMemoryHandling();
void updateBuffer(float sample, int &index);
bool saveBuffer(int &index);

#endif