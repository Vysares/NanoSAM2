#ifndef EDAC_H
#define EDAC_H 

/* - - - - - - Includes - - - - - - */
// C++ libraries
#include <algorithm>

// Other libraries

// NS2 config and utility headers
#include "config.hpp"

/* - - - - - - Bit Operation Macros - - - - - - */
// a=target variable, bit=bit number to act upon 0-n */
#define BIT_SET(a,bit) ((a) |= (1ULL<<(bit)))
#define BIT_CLEAR(a,bit) ((a) &= ~(1ULL<<(bit)))
#define BIT_FLIP(a,bit) ((a) ^= (1ULL<<(bit)))
#define BIT_CHECK(a,bit) (!!((a) & (1ULL<<(bit)))) // '!!' to make sure this returns 0 or 1

struct ErrorReport {
    bool errorDetected = false;
    int errorSize = 0;
    int errorPosition = -1;
};

/* - - - - - - Declarations - - - - - - */
void encodeScienceData(uint8_t *destination, uint16_t *buffer, unsigned long &timestamp);
void encodeHamming_72_64(uint8_t *hammingBlock, uint8_t *messageBlock);
void decodeHamming_72_64(uint8_t *hammingBlock, uint8_t *messageBlock);
ErrorReport scan(uint8_t *hammingBlock);
void printBlock(uint8_t *hammingBlock);
bool checkBit(uint8_t *byteArray, int index);
void assignBit(uint8_t *byteArray, int index, bool val);
void flipBit(uint8_t *byteArray, int index);

#endif