
#include "../headers/edac.hpp"


void Hamming_72_64(uint8_t *dataBlock) {
    //hammingBlock_t hammingBlock; // bitset to hold the data block with EDAC

    uint8_t *dataBlock;
    dataBlock = reinterpret_cast<uint8_t*>();

    int dataIdx = 0;
    for (int blockIdx = 0; blockIdx < HAMMING_BLOCK_SIZE; blockIdx++) {
        if (blockIdx & (blockIdx - 1) == 0) { // a real fancy way to check if blockIdx is a power of 2
            // reserve all indexes that are powers of two for parity bits 
            hammingBlock[blockIdx] = 0;
        } else {
            // insert the data into the hamming block
            hammingBlock[blockIdx] = dataBlock[dataIdx];
            dataIdx++;
        }
    }
} 

