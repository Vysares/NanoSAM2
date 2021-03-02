#ifndef ENCODEDFILE_H
#define ENCODEDFILE_H 

/* - - - - - - Includes - - - - - - */
// C++ libraries
#include <algorithm>
#include <cstring>
#include <vector>
#include <memory>

// Other libraries

// NS2 config and utility headers
#include "config.hpp"
#include "hammingBlock.hpp"


/* - - - - - - Class Definitions - - - - - - */

/* - ScrubReport -
*   Holds results of file scrub.
*   Members: numErrors, corrected, uncorrected 
*/
struct ScrubReport { 
    int numErrors = 0;      // number of errors detected
    int corrected = 0;      // number of single bit errors corrected
    int uncorrected = 0;    // number of errors detected but not corrected
};


/* - EncodedFile -
*   Container for encoded file contents
*/
class EncodedFile {
    private:
        // member variables
        HammingBlock m_blocks[MESSAGE_COUNT]; // vector of encoded hamming blocks
        uint8_t m_data[ENCODED_FILE_SIZE];  // array to hold encoded/decoded data
        uint16_t m_buffer[BUFFERSIZE];      // array to hold decoded buffer
        unsigned long m_timestamp;          // file timestamp

        // helper methods
        void decode();
    
    public:
        // constructors
        EncodedFile();
        EncodedFile(uint16_t *buffer, unsigned long &timestamp);
        EncodedFile(void *encodedData);
        
        // methods
        void encodeData(uint16_t *buffer, unsigned long &timestamp);
        void fill(void *encodedData);
        ScrubReport scrub();

        // getters
        uint8_t *getData();
        uint16_t *getBuffer();
        unsigned long getTimestamp();

        // for debugging
        void printBlock(int index);
        void injectError(int blockNum, int index);
};

#endif