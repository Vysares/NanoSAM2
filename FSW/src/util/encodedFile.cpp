/* encodedFile.cpp defines the EncodedFile class
 * Usage:
 *  An EncodedFile object is used to encode and decode science data.
 * 
 * Modules encompassed:
 *  Memory Scrubbing
 *
 * Additional files needed for compilation:
 *  config.hpp
 */

/* - - - - - - Includes - - - - - - */
// All libraries are put in edac.hpp
// NS2 headers
#include "../headers/encodedFile.hpp"


/* - - - - - - Default Constructor - - - - - - *
 * Usage:
 *  Constructs an empty EncodedFile
 *  
 * Inputs:
 *  None
 */
EncodedFile::EncodedFile() {
    m_timestamp = 0;
}

/* - - - - - - Constructor (science data) - - - - - - *
 * Usage:
 *  Constructs an EncodedFile and fills it with new science data
 *  
 * Inputs:
 *  buffer - pointer to buffer of photodiode data
 *  timestamp - file timestamp
 */
EncodedFile::EncodedFile(uint16_t *buffer, unsigned long &timestamp) : EncodedFile::EncodedFile() {
    encodeData(buffer, timestamp);
}

/* - - - - - - Constructor (encoded data) - - - - - - *
 * Usage:
 *  Constructs an EncodedFile and fills it with already-encoded data
 *  
 * Inputs:
 *  encodedData - pointer to encoded file data, typecast to void*
 */
EncodedFile::EncodedFile(void *encodedData) : EncodedFile::EncodedFile() {
    fill(encodedData);
}

/* - - - - - - encodeData - - - - - - *
 * Usage:
 *  Fills the file with science data and encodes the file
 *  
 * Inputs:
 *  buffer - pointer to buffer of photodiode data
 *  timestamp - file timestamp
 * 
 * Outputs:
 *  None
 */
void EncodedFile::encodeData(uint16_t *buffer, unsigned long &timestamp) {
    
    memset(m_data, 0, ENCODED_FILE_SIZE); // clear the data array
    memcpy(m_data, buffer, BUFFER_MEMSIZE); // copy buffer to data array
    memcpy(m_data + BUFFER_MEMSIZE, &timestamp, TIMESTAMP_SIZE); // copy timestamp to data array

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
void EncodedFile::fill(void *encodedData) {
    // overwrite each block
    for (int blockNum = 0; blockNum < MESSAGE_COUNT; blockNum++) { // for each block...
        m_blocks[blockNum].fill(encodedData + blockNum * HAMMING_BLOCK_SIZE); // overwrite the block with new data
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
ScrubReport EncodedFile::scrub() {
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
uint8_t *EncodedFile::getData() {
    memset(m_data, 0, ENCODED_FILE_SIZE); // clear the data array
    for (int blockNum = 0; blockNum < MESSAGE_COUNT; blockNum++) { // for each block...
        // copy the encoded block contents to the data array
        memcpy(m_data + blockNum * HAMMING_BLOCK_SIZE, m_blocks[blockNum].getBlock(), HAMMING_BLOCK_SIZE);
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
void EncodedFile::decode() { 
    memset(m_data, 0, ENCODED_FILE_SIZE); // clear the data array
    // decode the data to the data array 
    for (int blockNum = 0; blockNum < MESSAGE_COUNT; blockNum++) { // for each block...
        // copy the decoded message to the data array
        memcpy(m_data + blockNum * MESSAGE_SIZE, m_blocks[blockNum].getMessage(), MESSAGE_SIZE);
    }
    // extract the buffer and timestamp
    memcpy(m_buffer, m_data, BUFFER_MEMSIZE);
    memcpy(&m_timestamp, m_data + BUFFER_MEMSIZE, TIMESTAMP_SIZE);
}

/* - - - - - - getBuffer - - - - - - *
 * Usage:
 *  Returns a pointer to the decoded buffer contents
 *  
 * Inputs:
 *  None
 * 
 * Outputs:
 *  Pointer to readable array containing buffer data, type uint16_t*
 */
uint16_t *EncodedFile::getBuffer() {
    decode(); // decode the file contents
    return m_buffer;
}

/* - - - - - - getTimestamp - - - - - - *
 * Usage:
 *  Returns a pointer to the decoded file timestamp
 *  
 * Inputs:
 *  None
 * 
 * Outputs:
 *  timestamp
 */
unsigned long EncodedFile::getTimestamp() {
    decode(); // decode the file contents
    return m_timestamp;
}


/* For debugging: */

void EncodedFile::printBlock(int blockNum) {
    m_blocks[blockNum].printBlock();
}

void EncodedFile::injectError(int blockNum, int index) {
    m_blocks[blockNum].injectError(index);
}