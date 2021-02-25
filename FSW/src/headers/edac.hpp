#ifndef EDAC_H
#define EDAC_H 

/* - - - - - - Includes - - - - - - */
// C++ libraries
#include <algorithm>

// Other libraries

// NS2 config and utility headers
#include "config.hpp"

/* - - - - - - Bit Operation Macros - - - - - - */
// a = target variable, bit = bit number to act on (0 indexed)
#define BIT_SET(a,bit) ((a) |= (1ULL<<(bit)))       // sets bit to 1
#define BIT_CLEAR(a,bit) ((a) &= ~(1ULL<<(bit)))    // clears bit to 0
#define BIT_FLIP(a,bit) ((a) ^= (1ULL<<(bit)))      // flips bit
#define BIT_CHECK(a,bit) (!!((a) & (1ULL<<(bit))))  // returns bit value, 0 or 1


/* - - - - - - Structs - - - - - - */

/* - ErrorReport -
*   Holds error info for a single hamming block.
*   Members: size, position
*/
struct ErrorReport { 
    int size = 0;      // size of detected error, 0 for no detected error
    int position = -1; // position of error if errorSize is 1, -1 otherwise
};

/* - ScrubReport -
*   Holds results of file scrub.
*   Members: numErrors, corrected, uncorrected 
*/
struct ScrubReport { 
    int numErrors = 0;      // number of errors detected
    int corrected = 0;      // number of single bit errors corrected
    int uncorrected = 0;    // number of errors detected but not corrected
};

/* - DecodedData -
*   Holds readable science data and scrub report from decoded file.
*   Members: photdiode16, timestamp, scrubInfo 
*/
struct DecodedData { 
    uint16_t photoData[BUFFERSIZE] = {};  // array of photodiode ADC bin numbers
    unsigned long timestamp = 0UL;          // timestamp
    ScrubReport scrubInfo;                  // contains info on errors
};


/* - - - - - - Declarations - - - - - - */
void encodeFile(uint8_t *destination, uint16_t *buffer, unsigned long timestamp);
DecodedData decodeFile(uint8_t *encodedData);
void encodeHamming_72_64(uint8_t *hammingBlock, uint8_t *messageBlock);
void decodeHamming_72_64(uint8_t *hammingBlock, uint8_t *messageBlock);
ScrubReport scrubFile(uint8_t *encodedData);
ErrorReport scanBlock(uint8_t *hammingBlock);
ErrorReport correctBlock(uint8_t *hammingBlock);
void printBlock(uint8_t *hammingBlock);
bool checkBit(uint8_t *byteArray, int index);
void assignBit(uint8_t *byteArray, int index, bool val);
void flipBit(uint8_t *byteArray, int index);

#endif