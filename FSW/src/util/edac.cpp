// All libraries are put in edac.hpp
// NS2 headers
#include "../headers/edac.hpp"


void encodeScienceData(char *destination, uint16_t *buffer, unsigned long &timestamp) {

    /* copy all data to a single byte array */
    // temporary arrays to store photodiode and timing data
    uint8_t bufferBytes[BUFFERSIZE * sizeof(uint16_t)];
    uint8_t timestampBytes[sizeof(timestamp)];

    // copy data to temporary arrays
    memcpy(bufferBytes, &buffer, BUFFERSIZE * sizeof(uint16_t)); // copy timestamp data to array of bytes
    memcpy(timestampBytes, &timestamp, sizeof(timestamp)); // copy timestamp data to array of bytes

    // declare byte array for all raw data
    int rawDataSize = sizeof(bufferBytes) + sizeof(timestampBytes); // total number of bytes across all data
    uint8_t rawData[rawDataSize]; // rawData holds all photodiode data and timestamp data

    // copy data to rawData from temporary arrays
    std::copy(bufferBytes, bufferBytes + sizeof(bufferBytes), rawData); 
    std::copy(timestampBytes, timestampBytes + sizeof(timestampBytes), rawData + sizeof(bufferBytes)); 


    /* divide rawData into message blocks */
    uint8_t messageBlock[MESSAGE_BLOCK_SIZE];

    for (int i = 0; i < rawDataSize; i++) { // loop through every byte of data
        
        int messageIdx = i % MESSAGE_BLOCK_SIZE; // wraps from 0 to 7 as i increases
        std::copy(rawData + i, rawData + i + 1, messageBlock + messageIdx); // copy byte from rawData into data block

        if (i != 0 && messageIdx == 0) { // if dataBlock is filled...    
            // generate new block for encoded data
            uint8_t hammingBlock[HAMMING_BLOCK_SIZE] = {};   // empty block with room for data and parity bits
            encodeHamming_72_64(hammingBlock, messageBlock);  // fill hammingBlock with encoded data
        }
    }
}


void encodeHamming_72_64(uint8_t *hammingBlock, uint8_t *messageBlock) {

    uint8_t indexParity = 0; // stores the parity of all indices with a set bit
    int totalSetBits = 0;
    
    /*  */
    int messageIdx = 0;
    for (int hammingIdx = 0; hammingIdx < HAMMING_BLOCK_SIZE * 0b1000; hammingIdx++) { // for each bit in the hamming block

        if (( hammingIdx & (hammingIdx - 1) ) != 0) { // if current index is not power of 2
            // copy bit from message block to hamming block
            assignBit(hammingBlock, hammingIdx, checkBit(messageBlock, messageIdx));
            
            // if message bit is set, include in calculation of indexParity
            if (checkBit(messageBlock, messageIdx)) {
                indexParity ^= hammingIdx;
                totalSetBits++;
            }
            messageIdx++;

        } else {            
            // reserve the bit for parity    
        }
    }
    
    /* Set parity bits in hamming block 
    *   copies each bit of index parity to the hamming block at the index equal to its value.
    *   e.g. for an index parity of 0001 0011, parity bits will be set at indices 1,2,16 in the hamming block 
    *   the resulting index parity is 0 after parity bits are set */
    for (int n = 0; n < 0b1000 - 1; n++) { // for each bit in indexParity
        
        int idx = pow(2, n); // index of parity bit in hamming block
        assignBit(hammingBlock, idx, BIT_CHECK(indexParity, n)); // set parity bit in hamming block
    }
    
    // find total block parity and put it in index 0
    bool blockParity = !!(totalSetBits % 2);
    assignBit(hammingBlock, 0, blockParity);
} 


void decodeHamming_72_64(uint8_t *hammingBlock, uint8_t *messageBlock) {

    uint8_t indexParity = 0; // stores the parity of all indices with a set bit
    
    /* Extract message from block */
    int messageIdx = 0;
    for (int hammingIdx = 0; hammingIdx < HAMMING_BLOCK_SIZE * 0b1000; hammingIdx++) { // for each bit in the hamming block

        if (( hammingIdx & (hammingIdx - 1) ) != 0) { // if current index is not power of 2, and thus not a parity bit
            // copy bit from hamming block to message block
            assignBit(messageBlock, messageIdx, checkBit(hammingBlock, hammingIdx));
            messageIdx++;

        } else {            
            // current bit is a parity bit   
        }
    }
} 


ErrorReport scan(uint8_t *hammingBlock) {
    ErrorReport report;
    uint8_t indexParity = 0;
    int totalSetBits = 0;

    for (int hammingIdx = 0; hammingIdx < HAMMING_BLOCK_SIZE * 0b1000; hammingIdx++) { // for each bit in the hamming block
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
            report.errorDetected = true;
            report.errorSize = 2;
        }
    } else {
        // single bit error detected
        report.errorDetected = true;
        report.errorSize = 1;
        report.errorPosition = indexParity; // The position of the error is just the parity!
    }
    return report;
}


void printBlock(uint8_t *hammingBlock) {
    for (int idx = 0; idx < HAMMING_BLOCK_SIZE * 0b1000; idx++) {
        if (idx % 0b1000 == 0 && idx != 0) {
            Serial.println();
        }
        Serial.print(checkBit(hammingBlock, idx));
    }
    Serial.println("\n");
}

bool checkBit(uint8_t *byteArray, int index) {
    int byteIdx = index / 0b1000;
    int subBit = index % 0b1000;

    return BIT_CHECK(byteArray[byteIdx], subBit);
}

void assignBit(uint8_t *byteArray, int index, bool val) {
    int byteIdx = index / 0b1000;
    int subBit = index % 0b1000;

    BIT_CLEAR(byteArray[byteIdx], subBit);
    if (val) {
        BIT_SET(byteArray[byteIdx], subBit);
    }
}

void flipBit(uint8_t *byteArray, int index) {
    assignBit(byteArray, index, !checkBit(byteArray, index));
}



