/* hammingBlock.cpp defines the HammingBlock class
 * Usage:
 *  A HammingBlock object is a container for an 8 byte message, which is encoded into a 9 byte
 *  block using a (72,64) Hamming code.
 *  A HammingBlock can encode/decode itself, scan its contents for errors, and correct single bit errors.
 * 
 * Modules encompassed:
 *  Memory Scrubbing
 *  Program EDAC
 *
 * Additional files needed for compilation:
 *  config.hpp
 *  
 */

/* - - - - - - Includes - - - - - - */
// All libraries are put in edac.hpp
// NS2 headers
#include "../headers/hammingBlock.hpp"


/* - - - - - - HammingBlock Constructor - - - - - - *
* Inputs:
*   None
*/
HammingBlock::HammingBlock() {
    memset(m_block, 0, HAMMING_BLOCK_SIZE);
}

/* - - - - - - encodeMessage - - - - - - *
 * Usage:
 *  Encodes an 8 byte message into the block
 * 
 * Inputs:
 *  message - pointer to a single 8 byte message block, typecast to void*
 *  
 * Outputs:
 *  None
 */
void HammingBlock::encodeMessage(void *message) {
    
    memset(m_block, 0, HAMMING_BLOCK_SIZE); // clear the destination block
    uint8_t indexParity = 0; // stores the parity of all indices with a set bit
    int totalSetBits = 0;
    
    /* copy message into hamming block */
    int messageIdx = 0;
    for (int hammingIdx = 0; hammingIdx < HAMMING_BLOCK_SIZE * 8; hammingIdx++) { // for each bit in the hamming block...

        if (( hammingIdx & (hammingIdx - 1) ) != 0) { // if current index is not power of 2...
            // copy bit from message block to hamming block
            assignBit(m_block, hammingIdx, checkBit(message, messageIdx));
            
            // if message bit is 1, include in calculation of indexParity
            if (checkBit(message, messageIdx)) {
                indexParity ^= hammingIdx;
            }
            messageIdx++;
        }
    }
    
    /* Set parity bits in hamming block 
    *   copies each bit of index parity to the hamming block at the index equal to its value.
    *   e.g. for an index parity of 0001 0011, parity bits will be set at indices 1,2,16 in the hamming block */
    for (int n = 0; n < 0b1000 - 1; n++) { // for each bit in indexParity...
        int idx = pow(2, n); // find corresponding index in hamming block
        assignBit(m_block, idx, BIT_CHECK(indexParity, n)); // set parity bit in hamming block
    }

    // find the total block parity and put it in index 0
    for (int hammingIdx = 1; hammingIdx < HAMMING_BLOCK_SIZE * 8; hammingIdx++) { // for each bit in the hamming block...
        if (checkBit(m_block, hammingIdx)) {
                totalSetBits++;
        }
    }
    bool blockParity = !!(totalSetBits % 2);
    assignBit(m_block, 0, blockParity);
} 

/* - - - - - - getMessage - - - - - - *
 * Usage:
 *  Retrieves and returns the decoded message
 * 
 * Inputs:
 *  None
 * 
 * Outputs:
 *  pointer to the decoded message, type uint8_t*
 */
uint8_t *HammingBlock::getMessage() {
    memset(m_message, 0, MESSAGE_SIZE); // clear last known message
    // Extract message from block
    int messageIdx = 0;
    for (int hammingIdx = 0; hammingIdx < HAMMING_BLOCK_SIZE * 8; hammingIdx++) { // for each bit in the hamming block...
        if (( hammingIdx & (hammingIdx - 1) ) != 0) { // if current index is not power of 2, and thus not a parity bit...
            // copy bit from hamming block to destination message block
            assignBit(m_message, messageIdx, checkBit(m_block, hammingIdx));
            messageIdx++;
        }
    }
    return m_message;
}

/* - - - - - - scanBlock - - - - - - *
 * Usage:
 *  Scans the block for errors.
 * 
 * Inputs:
 *  None
 *  
 * Outputs:
 *  ErrorReport struct with size of detected error and error position. Returns a position of -1 for errors not of size 1
 */
ErrorReport HammingBlock::scanBlock() {
    ErrorReport errorInfo;
    uint8_t indexParity = 0;
    int totalSetBits = 0;

    // calculate index parity
    for (int hammingIdx = 1; hammingIdx < HAMMING_BLOCK_SIZE * 8; hammingIdx++) { // for each bit in the hamming block...
        if (checkBit(m_block, hammingIdx)) {
                indexParity ^= hammingIdx;
                totalSetBits++;
        }
    }
    // calculate block parity
    bool blockParity = !!(totalSetBits % 2);
    Serial.println(totalSetBits);
    Serial.println(checkBit(m_block, 0));
    Serial.println(blockParity);
    Serial.println(checkBit(m_block, 0) == blockParity);
    // determine error type
    if (checkBit(m_block, 0) == blockParity) {
        if (indexParity == 0) {
            // no error detected
        } else {
            // two bit error detected
            errorInfo.size = 2;
        }
    } else {
        // single bit error detected
        errorInfo.size = 1;
        errorInfo.position = indexParity; // The parity tells us the index of the error!!!
    }
    return errorInfo;
}

/* - - - - - - correctBlock - - - - - - *
 * Usage:
 *  Scans the block for errors and corrects single bit errors
 * 
 * Inputs:
 *  None
 *  
 * Outputs:
 *  ErrorReport struct with size of detected error and error position. Returns a position of -1 for errors not of size 1
 */
ErrorReport HammingBlock::correctBlock() {
    // scan the block for errors
    ErrorReport errorInfo;
    errorInfo = scanBlock(); 
    // correct single bit errors
    if (errorInfo.size == 1) {
        flipBit(m_block, errorInfo.position);
    }
    return errorInfo;
}

/* - - - - - - fill - - - - - - *
 * Usage:
 *  fills the block with new data, overwriting its contents
 * 
 * Inputs:
 *  newData - pointer to new data with which to fill the block, typecast to void*
 *  
 * Outputs:
 *  None
 */
void HammingBlock::fill(void *newData) {
    memcpy(m_block, newData, HAMMING_BLOCK_SIZE);
}

/* - - - - - - clear - - - - - - *
 * Usage:
 *  clears each bit in the block to 0
 * 
 * Inputs:
 *  None
 * Outputs:
 *  None
 */
void HammingBlock::clear() {
    memset(m_block, 0, HAMMING_BLOCK_SIZE);
}

/* - - - - - - checkBit (private) - - - - - - *
 * Usage:
 *  Returns the value of a single bit in at the specified index
 * 
 * Inputs:
 *  dst - a pointer to the data of interest, typecast to void*
 *  index - position of bit to check
 *  
 * Outputs:
 *  true if the bit is 1, false if the bit is 0
 */
static bool HammingBlock::checkBit(void *dst, int index) {
    uint8_t *byteArray = static_cast<uint8_t *>(dst);
    int byteIdx = index / 0b1000;
    int subBit = index % 0b1000;
    return BIT_CHECK(byteArray[byteIdx], subBit);
}

/* - - - - - - assignBit (private) - - - - - - *
 * Usage:
 *  Assigns a single bit to the specified value
 * 
 * Inputs:
 *  dst - a pointer to the data of interest, typecast to void*
 *  index - position of bit to flip 0-n
 *  val - value to assign bit, 0 or 1 
 *  
 * Outputs:
 *  None
 */
static void HammingBlock::assignBit(void *dst, int index, bool val) {
    uint8_t *byteArray = static_cast<uint8_t *>(dst);
    int byteIdx = index / 0b1000;
    int subBit = index % 0b1000;
    BIT_CLEAR(byteArray[byteIdx], subBit);
    if (val) {
        BIT_SET(byteArray[byteIdx], subBit);
    }
}

/* - - - - - - flipBit (private) - - - - - - *
 * Usage:
 *  Flips a single bit at the specified index
 * 
 * Inputs:
 *  dst - a pointer to the data of interest, typecast to void*
 *  index - position of bit to flip 0-n
 *  
 * Outputs:
 *  None
 */
static void HammingBlock::flipBit(void *dst, int index) {
    assignBit(dst, index, !checkBit(dst, index));
}



/* For Debugging: */

void HammingBlock::printBlock() {
    // for (int i = 0; i < HAMMING_BLOCK_SIZE; i ++) { Serial.println(m_block[i]); }
    for (int idx = 0; idx < HAMMING_BLOCK_SIZE * 0b1000; idx++) {
        if (idx % 0b1000 == 0 && idx != 0) {
            Serial.println();
        }
        Serial.print(checkBit(m_block, idx));
    }
    Serial.println("\n");
}

void HammingBlock::injectError(int index) {
    flipBit(m_block, index);
}







