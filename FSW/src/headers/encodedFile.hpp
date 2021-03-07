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
template <size_t TMessageCount> 
class EncodedFile {
    protected:
        // member variables
        static const size_t MESSAGE_COUNT = TMessageCount;
        static const size_t MEMSIZE = TMessageCount * HAMMING_BLOCK_SIZE;
        static const size_t DECODED_MEMSIZE = TMessageCount * MESSAGE_SIZE;
        HammingBlock m_blocks[MESSAGE_COUNT]; // vector of encoded hamming blocks
        uint8_t m_data[MEMSIZE];  // array to hold encoded/decoded data

        // helper methods
        void decode();
    
    public:
        // constructors
        EncodedFile();
        EncodedFile(void *encodedData);
        
        // methods
        void encodeData(void *src);
        void fill(void *encodedData);
        ScrubReport scrub();

        // getters
        uint8_t *getData();
        
        // for debugging
        void printBlock(int index);
        void injectError(int blockNum, int index);
};


/* - - - - - - Default Constructor - - - - - - *
 * Usage:
 *  Constructs an empty EncodedFile
 *  
 * Inputs:
 *  None
 */
template <size_t TMessageCount>
EncodedFile<TMessageCount>::EncodedFile() { }

/* - - - - - - Constructor (encoded data) - - - - - - *
 * Usage:
 *  Constructs an EncodedFile and fills it with already-encoded data
 *  
 * Inputs:
 *  encodedData - pointer to encoded file data, typecast to void*
 */
template <size_t TMessageCount>
EncodedFile<TMessageCount>::EncodedFile(void *encodedData) {
    fill(encodedData);
}

template <size_t TMessageCount>
void EncodedFile<TMessageCount>::encodeData(void *src) {
    memset(m_data, 0, MEMSIZE); // clear the data array
    memcpy(m_data, src, DECODED_MEMSIZE); // copy buffer to data array

    /* encode the data */ 
    for (int blockNum = 0; blockNum < MESSAGE_COUNT; blockNum++) { // for each block...
        // copy a chunk of unencoded data into a temporary message block
        uint8_t message[MESSAGE_SIZE]; 
        memcpy(message, m_data + blockNum * MESSAGE_SIZE, MESSAGE_SIZE);

        // encode the message
        m_blocks[blockNum].encodeMessage(message);
    }
}

/* - - - - - - fill - - - - - - *
 * Usage:
 *  Fills the file with encoded file data
 *  
 * Inputs:
 *  encodedData - pointer to already-encoded file data, typecast to void* 
 * 
 * Outputs:
 *  None
 */
template <size_t TMessageCount>
void EncodedFile<TMessageCount>::fill(void *encodedData) {
    // overwrite each block
    // for (int blockNum = 0; blockNum < MESSAGE_COUNT; blockNum++) { // for each block...
    //     m_blocks[blockNum].fill(encodedData + blockNum * HAMMING_BLOCK_SIZE); // overwrite the block with new data
    // }

    for (int blockNum = 0; blockNum < MESSAGE_COUNT; blockNum++) { // for each block...
        uint8_t unlacedBlockData[HAMMING_BLOCK_SIZE]; // temporary array to store block data
        // de-interlace the encoded data
        for (int blockBit = 0; blockBit < HAMMING_BLOCK_SIZE * 8; blockBit++) {
            int bitIdx = blockNum + (blockBit * MESSAGE_COUNT); // bit index of encoded data belonging to block
            bool val = HammingBlock::checkBit(encodedData, bitIdx); // get bit value
            HammingBlock::assignBit(unlacedBlockData, blockBit, val); // assign bit to temporary block
        }
        m_blocks[blockNum].fill(unlacedBlockData); // fill block with unlaced data
    }
}

/* - - - - - - scrub - - - - - - *
 * Usage:
 *  Scans the file for errror and corrects single bit errors.
 *  Corrupted blocks that cannot be corrected are cleared.
 *  
 * Inputs:
 *  None
 * 
 * Outputs:
 *  A ScrubReport struct containing counts of found and corrected errors
 */
template <size_t TMessageCount>
ScrubReport EncodedFile<TMessageCount>::scrub() {
    ScrubReport scrubInfo;

    /* scan and correct each block */
    for (int blockNum = 0; blockNum < MESSAGE_COUNT; blockNum++) { // for each block...
        // correct the block and get an error report
        ErrorReport errorInfo = m_blocks[blockNum].correctBlock();

        // if uncorrectable error detected, clear the block
        if (errorInfo.size >= 2) {
            m_blocks[blockNum].clear();
        } 
        // update scrub report with latest error report
        scrubInfo.numErrors += !!errorInfo.size;
        scrubInfo.corrected += errorInfo.size == 1;
        scrubInfo.uncorrected += errorInfo.size > 1;
    }
    return scrubInfo;
}

/* - - - - - - getData - - - - - - *
 * Usage:
 *  Returns a pointer to encoded file data
 *  
 * Inputs:
 *  None
 * 
 * Outputs:
 *  Pointer to encoded data, type uint8_t*
 */
template <size_t TMessageCount>
uint8_t *EncodedFile<TMessageCount>::getData() {
    memset(m_data, 0, MEMSIZE); // clear the data array
    for (int blockNum = 0; blockNum < MESSAGE_COUNT; blockNum++) { // for each block...
        // Interlace each block, so that burst errors span several blocks
        for (int blockBit = 0; blockBit < HAMMING_BLOCK_SIZE * 8; blockBit++) {

            int bitIdx = blockNum + (blockBit * MESSAGE_COUNT); // index of data array to place encoded bit
            bool val = HammingBlock::checkBit(m_blocks[blockNum].getBlock(), blockBit); // value of bit to copy
            HammingBlock::assignBit(m_data, bitIdx, val); // assign bit to data array
        }
    }
    return m_data;
}

/* - - - - - - decode (private) - - - - - - *
 * Usage:
 *  Extracts readable science data from the file
 *  
 * Inputs:
 *  None
 * Outputs:
 *  None
 */
template <size_t TMessageCount>
void EncodedFile<TMessageCount>::decode() { 
    memset(m_data, 0, MEMSIZE); // clear the data array
    
    for (int blockNum = 0; blockNum < MESSAGE_COUNT; blockNum++) { // for each block...
        // copy the decoded message to the data array
        memcpy(m_data + blockNum * MESSAGE_SIZE, m_blocks[blockNum].getMessage(), MESSAGE_SIZE);
    }
}


/* For debugging: */
template <size_t TMessageCount>
void EncodedFile<TMessageCount>::printBlock(int blockNum) {
    m_blocks[blockNum].printBlock();
}

template <size_t TMessageCount>
void EncodedFile<TMessageCount>::injectError(int blockNum, int index) {
    m_blocks[blockNum].injectError(index);
}






class EncodedSciData : public EncodedFile<SCIDATA_MESSAGE_COUNT> {
    private:
        
        uint16_t m_buffer[BUFFERSIZE];      // array to hold decoded buffer
        unsigned long m_timestamp = MEMSIZE;         // file timestamp

    public:
        EncodedSciData();
        EncodedSciData(uint16_t *buffer, unsigned long &timestamp);
        EncodedSciData(void *encodedData);

        void encodeData(uint16_t *buffer, unsigned long &timestamp);
        uint16_t *getBuffer();
        unsigned long getTimestamp();
};


#endif