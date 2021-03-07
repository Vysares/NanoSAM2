/* EncodedSciData.cpp defines the EncodedSciData class
 * Usage:
 *  An EncodedSciData object is used to encode and decode science data.
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
#include "../headers/encodedSciData.hpp"


/* - - - - - - Constructor (science data) - - - - - - *
 * Usage:
 *  Constructs an EncodedSciData and fills it with new science data
 *  
 * Inputs:
 *  buffer - pointer to buffer of photodiode data
 *  timestamp - file timestamp
 */
EncodedSciData::EncodedSciData(uint16_t *buffer, unsigned long &timestamp) {
    encodeData(buffer, timestamp);
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
void EncodedSciData::encodeData(uint16_t *buffer, unsigned long &timestamp) {
    
    uint8_t rawData[DECODED_MEMSIZE];
    memcpy(rawData, buffer, BUFFER_MEMSIZE); // copy buffer to data array
    memcpy(rawData + BUFFER_MEMSIZE, &timestamp, TIMESTAMP_SIZE); // copy timestamp to data array
    EncodedFile<SCIDATA_MESSAGE_COUNT>::encodeData(rawData);
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
uint16_t *EncodedSciData::getBuffer() {
    decode(); // decode the file contents
    memcpy(m_buffer, m_data, BUFFER_MEMSIZE);
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
unsigned long EncodedSciData::getTimestamp() {
    decode(); // decode the file contents
    memcpy(&m_timestamp, m_data + BUFFER_MEMSIZE, TIMESTAMP_SIZE);
    return m_timestamp;
}

