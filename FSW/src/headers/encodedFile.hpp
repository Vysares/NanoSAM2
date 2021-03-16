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
template <size_t N> 
class EncodedFile {
    public:
        static const int DECODED_MEMSIZE = N;
        static const int MESSAGE_COUNT = (DECODED_MEMSIZE / HammingBlock::MSG_SIZE) + !!(DECODED_MEMSIZE % HammingBlock::MSG_SIZE);
        static const int MEMSIZE = MESSAGE_COUNT * HammingBlock::BLOCK_SIZE;
        
        // constructors
        EncodedFile() { }
        EncodedFile(void *src);
        
        // methods
        void encodeData(void *src);
        void fill(void *encodedData);
        ScrubReport scrub();

        // getters
        uint8_t *getData() { return m_data; }
        uint8_t *decode();
        
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
template <size_t N>
EncodedFile<N>::EncodedFile(void *src) {
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
template <size_t N>
void EncodedFile<N>::encodeData(void *src) {
    memset(m_data, 0, MEMSIZE); // clear the data array
   // memcpy(m_data, src, DECODED_MEMSIZE); // copy buffer to data array

    /* encode the data in blocks */ 
    for (int blockNum = 0; blockNum < MESSAGE_COUNT; blockNum++) { // for each block...
        // copy a chunk of unencoded data into a temporary message block
        uint8_t message[HammingBlock::MSG_SIZE]; 
        memcpy(message, static_cast<uint8_t*>(src) + blockNum * HammingBlock::MSG_SIZE, HammingBlock::MSG_SIZE);

        // encode the message
        m_blocks[blockNum].encodeMessage(message);
    }

    /* interleave each block */
    for (int blockNum = 0; blockNum < MESSAGE_COUNT; blockNum++) { // for each block...
        // Interlace each block, so that burst errors span several blocks
        for (int blockBit = 0; blockBit < HammingBlock::BLOCK_SIZE * 8; blockBit++) {

            int bitIdx = blockNum + (blockBit * MESSAGE_COUNT); // index of data array to place encoded bit
            bool val = checkBit(m_blocks[blockNum].getBlock(), blockBit); // value of bit to copy
            assignBit(m_data, bitIdx, val); // assign bit to data array
        }
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
template <size_t N>
void EncodedFile<N>::fill(void *encodedData) {

    for (int blockNum = 0; blockNum < MESSAGE_COUNT; blockNum++) { // for each block...
        uint8_t unlacedBlockData[HammingBlock::BLOCK_SIZE]; // temporary array to store block data
        // de-interlace the encoded data
        for (int blockBit = 0; blockBit < HammingBlock::BLOCK_SIZE * 8; blockBit++) {
            int bitIdx = blockNum + (blockBit * MESSAGE_COUNT); // bit index of encoded data belonging to block
            bool val = checkBit(encodedData, bitIdx); // get bit value
            assignBit(unlacedBlockData, blockBit, val); // assign bit to temporary block
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
template <size_t N>
ScrubReport EncodedFile<N>::scrub() {
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
template <size_t N>
uint8_t *EncodedFile<N>::decode() { 
    static uint8_t decodedData[DECODED_MEMSIZE];
    memset(decodedData, 0, DECODED_MEMSIZE); // clear the data array
    
    for (int blockNum = 0; blockNum < MESSAGE_COUNT; blockNum++) { // for each block...
        // copy the decoded message to the data array
        memcpy(decodedData + blockNum * HammingBlock::MSG_SIZE, m_blocks[blockNum].getMessage(), HammingBlock::MSG_SIZE);
    }
    return decodedData;
}


/* For debugging: */
template <size_t N>
void EncodedFile<N>::printBlock(int blockNum) {
    m_blocks[blockNum].printBlock();
}

template <size_t N>
void EncodedFile<N>::injectError(int blockNum, int index) {
    m_blocks[blockNum].injectError(index);
}

#endif