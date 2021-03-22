#ifndef ENCODEDSCI_H
#define ENCODEDSCI_H 

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries

// NS2 config and utility headers
#include "config.hpp"
#include "encodedFile.hpp"


/* - - - - - - Class Declaration - - - - - - */

class EncodedSciData : public EncodedFile<SCIDATA_RAW_MEMSIZE> {
    private:
        // member variables
        uint16_t m_buffer[BUFFERSIZE];        // array to hold decoded buffer
        unsigned long m_timestamp = MEMSIZE;  // file timestamp

    public:
        // constructors
        EncodedSciData() { }
        EncodedSciData(uint16_t *buffer, unsigned long &timestamp);

        // public methods
        void encodeData(uint16_t *buffer, unsigned long &timestamp);
        uint16_t *getBuffer();
        unsigned long getTimestamp();
};

#endif