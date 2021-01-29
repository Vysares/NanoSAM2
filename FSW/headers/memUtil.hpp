#ifndef MEMUTIL_H
#define MEMUTIL_H 

/* - - - - - - Includes - - - - - - */
// C++ libraries
#include <string>

// Other libraries
#include <SerialFlash.h> // for file I/O to flash modules
//#include <SPI.h> // will probably use this shortly, but it is not in use yet

// NS2 headers


/* - - - - - - Declarations - - - - - - */
void dataProcessing();
void scienceMemoryHandling();
void updateBuffer(float dataBuffer[], float sample, int &index);
bool saveBuffer(float dataBuffer[], int &index);
void fullErase(); // in case we need to manually erase all of the flash



#endif