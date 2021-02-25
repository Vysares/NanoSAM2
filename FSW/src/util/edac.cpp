/* edac.cpp handles error detection and correction of NS2 science and program data
 * Usage:
 *  
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
#include "../headers/edac.hpp"


/* - - - - - - Module Driver Functions - - - - - - */

/* - - - - - - decodeFile - - - - - - *
 * Usage:
 *  Encodes science data into encoded file.
 *  data at address specified by *encodedFile is overwritten by encoded data
 *  
 * Inputs:
 *  encodedFile - pointer to adress at which to write encoded data, must be of size FILESIZE
 *  buffer - pointer to buffer of photodiode data
 *  timestamp - file timestamp
 *  
 * Outputs:
 *  None
 */
void encodeFile(uint8_t *encodedFile, uint16_t *buffer, unsigned long timestamp) {

    /* copy all data to a single byte array */
    uint8_t decodedFile[DECODED_DATA_SIZE] = {}; // decodedFile holds all photodiode data and timestamp data
    memcpy(decodedFile, buffer, BUFFERSIZE * sizeof(uint16_t)); // copy buffer to decodedFile
    memcpy(decodedFile + BUFFERSIZE * sizeof(uint16_t), &timestamp, TIMESTAMP_SIZE); // copy timestamp data to decodedFile

    /* encode the data */
    for (int i = 0; i < MESSAGE_COUNT; i++) { // for each block in decoded file...
        // copy a chunk of decoded data into a new message block
        uint8_t message[MESSAGE_SIZE] = {}; 
        memcpy(message, decodedFile + i * MESSAGE_SIZE, MESSAGE_SIZE);

        // encode the message
        uint8_t hammingBlock[HAMMING_BLOCK_SIZE] = {};   // empty block with room for message and parity bits
        encodeHamming_72_64(hammingBlock, message); // fill hammingBlock with encoded data
        
        // copy encoded block to encoded data array
        memcpy(encodedFile + i * HAMMING_BLOCK_SIZE, hammingBlock, HAMMING_BLOCK_SIZE);
    }
}

/* - - - - - - decodeFile - - - - - - *
 * Usage:
 *  Takes a single window of encoded science data, scrubs it for errors, and returns readable data
 *  
 * Inputs:
 *  encodedFile - pointer to a file of encoded science data
 *  
 * Outputs:
 *  struct containing error info, and readable timestamp and photodiode data 
 */
DecodedData decodeFile(uint8_t *encodedFile) {
    
    Serial.println("d1");
    DecodedData data; // new struct to hold decoded data
    uint8_t decodedFile[DECODED_DATA_SIZE] = {}; // decodedFile holds all photodiode data and timestamp data
    Serial.println("d2");

    // scrub encoded file
    data.scrubInfo = scrubFile(encodedFile);
    Serial.println("d3");

    // extract data from file and decode 
    for (int fileIdx = 0; fileIdx < MESSAGE_COUNT; fileIdx++) { // for each block in file...
        //Serial.println(fileIdx);
        // extract a single hamming block from the encoded data
        uint8_t hammingBlock[MESSAGE_SIZE] = {}; 
        memcpy(hammingBlock, encodedFile + fileIdx * HAMMING_BLOCK_SIZE, HAMMING_BLOCK_SIZE);
        //Serial.println("d4");

        // decode the hamming block to get the message
        uint8_t message[MESSAGE_SIZE] = {}; 
        decodeHamming_72_64(hammingBlock, message);
        //Serial.println("d5");

        // copy message to decodedFile
        memcpy(decodedFile + fileIdx * MESSAGE_SIZE, message, MESSAGE_SIZE);
    }
    Serial.println("d6");
    // extract photdiode and timestamp data from decodedFile
    memcpy(data.photoData, decodedFile, sizeof(data.photoData));
    memcpy(&data.timestamp, decodedFile + sizeof(data.photoData), sizeof(data.timestamp));

    return data;
}


/* - - - - - - Module Helper Functions - - - - - - */

/* - - - - - - encodeHamming_72_64 - - - - - - *
 * Usage:
 *  Encodes a 64 bit message into a 72 bit hamming block.
 *  data at address specified by *hammingBlock is overwritten with encoded data
 * 
 * Inputs:
 *  hammingBlock - pointer to address at which to write encoded data, must be allocated 9 bytes
 *  message - pointer to a single unencoded message block
 *  
 * Outputs:
 *  None
 */
void encodeHamming_72_64(uint8_t *hammingBlock, uint8_t *message) {

    uint8_t indexParity = 0; // stores the parity of all indices with a set bit
    int totalSetBits = 0;
    
    /* copy message into hamming block */
    int messageIdx = 0;
    for (int hammingIdx = 0; hammingIdx < HAMMING_BLOCK_SIZE * 0b1000; hammingIdx++) { // for each bit in the hamming block...

        if (( hammingIdx & (hammingIdx - 1) ) != 0) { // if current index is not power of 2...
            // copy bit from message block to hamming block
            assignBit(hammingBlock, hammingIdx, checkBit(message, messageIdx));
            
            // if message bit is 1, include in calculation of indexParity
            if (checkBit(message, messageIdx)) {
                indexParity ^= hammingIdx;
                totalSetBits++;
            }
            messageIdx++;
        }
    }
    
    /* Set parity bits in hamming block 
    *   copies each bit of index parity to the hamming block at the index equal to its value.
    *   e.g. for an index parity of 0001 0011, parity bits will be set at indices 1,2,16 in the hamming block 
    *   the resulting index parity is 0 after parity bits are set */
    for (int n = 0; n < 0b1000 - 1; n++) { // for each bit in indexParity...
        int idx = pow(2, n); // index of parity bit in hamming block
        assignBit(hammingBlock, idx, BIT_CHECK(indexParity, n)); // set parity bit in hamming block
    }
    
    // find total block parity and put it in index 0
    bool blockParity = !!(totalSetBits % 2);
    assignBit(hammingBlock, 0, blockParity);
} 

/* - - - - - - decodeHamming_72_64 - - - - - - *
 * Usage:
 *  Decodes a 72 bit hamming block into a 64 bit message.
 *  data at address specified by *message is overwritten with decoded data
 * 
 * Inputs:
 *  hammingBlock - pointer to a single encoded hamming block
 *  message - pointer to address at which to write decoded data, must be allocated 8 bytes
 *  
 * Outputs:
 *  None
 */
void decodeHamming_72_64(uint8_t *hammingBlock, uint8_t *message) {
    // Extract message from block
    int messageIdx = 0;
    for (int hammingIdx = 0; hammingIdx < HAMMING_BLOCK_SIZE * 0b1000; hammingIdx++) { // for each bit in the hamming block...
        if (( hammingIdx & (hammingIdx - 1) ) != 0) { // if current index is not power of 2, and thus not a parity bit...
            // copy bit from hamming block to message block
            assignBit(message, messageIdx, checkBit(hammingBlock, hammingIdx));
            messageIdx++;
        }
    }
} 

/* - - - - - - scrubFile - - - - - - *
 * Usage:
 *  Scans encoded science data for errors, corrects single bit errors, returns report of findings.
 *  Blocks with uncorrectable errors have all their bits set
 * 
 * Inputs:
 *  encodedFile - pointer to a file of encoded science data
 *  
 * Outputs:
 *  struct with counts of total detected errors, corrected errors, and uncorrected errors
 */
ScrubReport scrubFile(uint8_t *encodedFile) {
    ScrubReport scrubInfo;
    
    for (int fileIdx = 0; fileIdx < MESSAGE_COUNT; fileIdx++) { // for each block in file...

        // extract a single hamming block from the encoded data
        uint8_t hammingBlock[MESSAGE_SIZE] = {}; 
        memcpy(hammingBlock, encodedFile + fileIdx * HAMMING_BLOCK_SIZE, HAMMING_BLOCK_SIZE);

        // Get error report and correct single bit errors in block
        ErrorReport errorInfo;
        errorInfo = correctBlock(hammingBlock);

        // update scrub report with latest error report
        scrubInfo.numErrors += !!errorInfo.size;
        scrubInfo.corrected += errorInfo.size == 1;
        scrubInfo.uncorrected += errorInfo.size > 1;

        // if uncorrectable error detected, overwrite block with its own file index

        // copy corrected block back into encoded file
        memcpy(encodedFile + fileIdx * HAMMING_BLOCK_SIZE, hammingBlock, HAMMING_BLOCK_SIZE);
    }
    return scrubInfo;
}

/* - - - - - - scanBlock - - - - - - *
 * Usage:
 *  Scans an encoded hamming block for errors.
 * 
 * Inputs:
 *  hammingBlock - pointer to a single encoded hamming block
 *  
 * Outputs:
 *  struct with size of detected error and error position. Returns a position of -1 for errors not of size 1
 */
ErrorReport scanBlock(uint8_t *hammingBlock) {
    ErrorReport errorInfo;
    uint8_t indexParity = 0;
    int totalSetBits = 0;

    for (int hammingIdx = 0; hammingIdx < HAMMING_BLOCK_SIZE * 0b1000; hammingIdx++) { // for each bit in the hamming block...
        if (checkBit(hammingBlock, hammingIdx)) {
                indexParity ^= hammingIdx;
                totalSetBits++;
        }
    }
    // calculate block parity
    bool blockParity = !!(totalSetBits % 2);

    // determine error type
    if (!!hammingBlock[0] == blockParity) {
        if (indexParity == 0) {
            // no error detected
        } else {
            // two bit error detected
            errorInfo.size = 2;
        }
    } else {
        // single bit error detected
        errorInfo.size = 1;
        errorInfo.position = indexParity; // The position of the error is just the parity!
    }
    return errorInfo;
}

/* - - - - - - correctBlock - - - - - - *
 * Usage:
 *  Scans an encoded hamming block for errors and corrects single bit errors
 * 
 * Inputs:
 *  hammingBlock - pointer to a single encoded hamming block
 *  
 * Outputs:
 *  struct with size of detected error and error position. Returns a position of -1 for errors not of size 1
 */
ErrorReport correctBlock(uint8_t *hammingBlock) {
    // scan the block for errors
    ErrorReport errorInfo;
    errorInfo = scanBlock(hammingBlock); 
    
    // correct single bit errors
    if (errorInfo.size == 1) {
        flipBit(hammingBlock, errorInfo.position);
    }
    return errorInfo;
}

/* - - - - - - printBlock - - - - - - *
 * Usage:
 *  Takes a hamming block and prints each of its bits.
 *  prints 8 bits (one byte) per line
 * 
 * Inputs:
 *  hammingBlock - pointer to a single hamming block
 *  
 * Outputs:
 *  None
 */
void printBlock(uint8_t *hammingBlock) {
    for (int idx = 0; idx < HAMMING_BLOCK_SIZE * 0b1000; idx++) {
        if (idx % 0b1000 == 0 && idx != 0) {
            Serial.println();
        }
        Serial.print(checkBit(hammingBlock, idx));
    }
    Serial.println("\n");
}

/* - - - - - - checkBit - - - - - - *
 * Usage:
 *  Returns the value of a single bit in byteArray at the specified index
 * 
 * Inputs:
 *  byteArray - a pointer to one or more bytes
 *  index - position of bit to check
 *  
 * Outputs:
 *  true if the bit is 1, false if the bit is 0
 */
bool checkBit(uint8_t *byteArray, int index) {
    int byteIdx = index / 0b1000;
    int subBit = index % 0b1000;
    return BIT_CHECK(byteArray[byteIdx], subBit);
}

/* - - - - - - assignBit - - - - - - *
 * Usage:
 *  Assigns a single bit in byteArray to the specified value
 * 
 * Inputs:
 *  byteArray - a pointer to one or more bytes
 *  index - position of bit to flip 0-n
 *  val - value to assign bit, 0 or 1
 *  
 * Outputs:
 *  None
 */
void assignBit(uint8_t *byteArray, int index, bool val) {
    int byteIdx = index / 0b1000;
    int subBit = index % 0b1000;
    
    BIT_CLEAR(byteArray[byteIdx], subBit);
    if (val) {
        BIT_SET(byteArray[byteIdx], subBit);
    }
}

/* - - - - - - flipBit - - - - - - *
 * Usage:
 *  Flips a single bit in byteArray at the specified index
 * 
 * Inputs:
 *  byteArray - a pointer to one or more bytes
 *  index - position of bit to flip 0-n
 *  
 * Outputs:
 *  None
 */
void flipBit(uint8_t *byteArray, int index) {
    assignBit(byteArray, index, !checkBit(byteArray, index));
}

