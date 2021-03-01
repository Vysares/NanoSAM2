#ifndef HAMMING_H
#define HAMMING_H

/* - - - - - - Includes - - - - - - */
// C++ libraries
#include <algorithm>
#include <cstring>
#include <vector>
#include <memory>

// Other libraries

// NS2 config and utility headers
#include "config.hpp"

/* - - - - - - Bit Operation Macros - - - - - - */
// a = target variable, bit = bit number to act on (0 indexed)
#define BIT_SET(a,bit) ( (a) |= (1ULL<<(bit)) )       // sets bit to 1
#define BIT_CLEAR(a,bit) ( (a) &= ~(1ULL<<(bit)) )    // clears bit to 0
#define BIT_FLIP(a,bit) ( (a) ^= (1ULL<<(bit)) )      // flips bit
#define BIT_CHECK(a,bit) ( !!((a) & (1ULL<<(bit))) )  // returns bit value, 0 or 1


/* - ErrorReport -
*   Holds error info for a single hamming block.
*   Members: size, position
*/
struct ErrorReport { 
    int size = 0;      // size of detected error, 0 for no detected error
    int position = -1; // position of error if errorSize is 1, -1 otherwise
};

/* - HammingBlock -
*   Container object for a single encoded hamming block */
class HammingBlock {
    private:
        // member variables
        uint8_t m_block[HAMMING_BLOCK_SIZE];
        uint8_t m_message[MESSAGE_SIZE];
        
        // helper methods
        bool checkBit(void *dst, int index);
        void assignBit(void *dst, int index, bool val);
        void flipBit(void *dst, int index);

    public:
        HammingBlock();
        
        // public methods
        void encodeMessage(void *message);
        ErrorReport scanBlock();
        ErrorReport correctBlock();
        void fill(void *newData);
        void clear();
        
        // getters
        uint8_t *getBlock() { return m_block; };
        uint8_t *getMessage();
        
        // for debugging
        void printBlock();
        void injectError(int index);
};

#endif