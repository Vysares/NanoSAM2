#ifndef ENCODEDFILE_H
#define ENCODEDFILE_H 

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries

// NS2 config and utility headers
#include "config.hpp"
#include "hammingBlock.hpp"


/* - - - - - - Class Declaration - - - - - - */

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
*   Container for encoded data. 
*/
template <size_t TDecodedSize> 
class EncodedFile {
    public:
        static const int DECODED_MEMSIZE = TDecodedSize;
        static const int MESSAGE_COUNT = (DECODED_MEMSIZE / MESSAGE_SIZE) + !!(DECODED_MEMSIZE % MESSAGE_SIZE);
        static const int MEMSIZE = MESSAGE_COUNT * HAMMING_BLOCK_SIZE;
        
        // constructors
        EncodedFile() { }
        EncodedFile(void *src);
        
        // methods
        void encodeData(void *src);
        void fill(void *encodedData);
        ScrubReport scrub();

        // getters
        uint8_t *getData();
        uint8_t *decode();
        int getMessageCount() { return MESSAGE_COUNT; }
        int getMemsize() { return MEMSIZE; }
        
        // for debugging
        void printBlock(int index);
        void injectError(int blockNum, int index);

    protected:
        // member variables
        HammingBlock m_blocks[MESSAGE_COUNT]; // vector of encoded hamming blocks
        uint8_t m_data[MEMSIZE];  // array to hold encoded/decoded data

};


/* = = = = = = = = = = = = = = = = = = = = =
 * = = = = = = Class Definition  = = = = = = 
 * = = = = = = = = = = = = = = = = = = = = = */
// The class definition must be in the header file since EncodedFile is a template class

/* - - - - - - Constructor (unencoded data) - - - - - - *
 * Usage:
 *  Constructs an EncodedFile and encodes it with data
 *  
 * Inputs:
 *  src - pointer to data to encode, typecast to void*
 */
template <size_t TDecodedSize>
EncodedFile<TDecodedSize>::EncodedFile(void *src) {
    encodeData(src);
}

/* - - - - - - encodeData - - - - - - *
 * Usage:
 *  Fills the file with science data and encodes the file
 *  
 * Inputs:
 *  src - pointer to data to encode, typecast to void*
 * 
 * Outputs:
 *  None
 */
template <size_t TDecodedSize>
void EncodedFile<TDecodedSize>::encodeData(void *src) {
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
template <size_t TDecodedSize>
void EncodedFile<TDecodedSize>::fill(void *encodedData) {

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
template <size_t TDecodedSize>
ScrubReport EncodedFile<TDecodedSize>::scrub() {
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
template <size_t TDecodedSize>
uint8_t *EncodedFile<TDecodedSize>::getData() {
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

/* - - - - - - decode - - - - - - *
 * Usage:
 *  Extracts all decoded messages from encoded file
 *  
 * Inputs:
 *  None
 * 
 * Outputs:
 *  pointer to decoded data, type uint8_t*
 */
template <size_t TDecodedSize>
uint8_t *EncodedFile<TDecodedSize>::decode() { 
    static uint8_t decodedData[DECODED_MEMSIZE];
    memset(decodedData, 0, DECODED_MEMSIZE); // clear the data array
    
    for (int blockNum = 0; blockNum < MESSAGE_COUNT; blockNum++) { // for each block...
        // copy the decoded message to the data array
        memcpy(decodedData + blockNum * MESSAGE_SIZE, m_blocks[blockNum].getMessage(), MESSAGE_SIZE);
    }
}


/* For debugging: */
template <size_t TDecodedSize>
void EncodedFile<TDecodedSize>::printBlock(int blockNum) {
    m_blocks[blockNum].printBlock();
}

template <size_t TDecodedSize>
void EncodedFile<TDecodedSize>::injectError(int blockNum, int index) {
    m_blocks[blockNum].injectError(index);
}

#endif