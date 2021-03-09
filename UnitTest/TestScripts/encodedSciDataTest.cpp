/* encodedSciDataTest.cpp tests the command module non-void functions
 * Usage:
 *  part of NS2 unit testing suite
 *  to be called in unitTestDriver.cpp
 *
 */


// NS2 includes
#include "../headers/config.hpp"
#include "../headers/encodedSciData.hpp"

// other includes 
#include <algorithm>
#include <stdlib.h>

/* - - - - - -  generateBuffer - - - - - - *
 * Usage:
 *  generates a buffer for testing
 * 
 * Inputs:
 *  switch index (test case)
 *  
 * Outputs:
 *  pointer to buffer
 */
uint16_t * generateBuffer(int testCase) {
    uint16_t buffer[BUFFERSIZE];
    switch (testCase){
        case 0: // all zeros
            std::fill_n(buffer, BUFFERSIZE, 0);
            return buffer;

        case 1: // all ones
            std::fill_n(buffer, BUFFERSIZE, 1);
            return buffer;

        case 2: // all max val
            std::fill_n(buffer, BUFFERSIZE, 65535); //2^16-1
            return buffer;

        case 3: // counting up from zero
            for (int i = 0; i<BUFFERSIZE; i++) {
                buffer[i] = i;
            }

        case 4: // ones at ends of buffer
            std::fill_n(buffer, BUFFERSIZE, 0);
            buffer[0] = 1; 
            buffer[BUFFERSIZE-1] = 1;
            return buffer;

        case 5: // zeros at ends of buffer
            std::fill_n(buffer, BUFFERSIZE, 1);
            buffer[0] = 0; 
            buffer[BUFFERSIZE-1] = 0;
            return buffer;

        case 6: // random data
            for (int i = 0; i < BUFFERSIZE; i++) {
                buffer[i] = rand() % 2^16; // from 0 to 65535
            }


        default: // exit case
            return nullptr;
    }
} 


/* - - - - - -  encodedSciDataTestMain - - - - - - *
 * Usage:
 * runs the EncodedSciData class unit tests, prints results over serial
 *  must be kept last in file since we are not using header structure for testing
 *  (or if you would rather use prototypes be my guest)
 * 
 * Inputs:
 *  none
 *  
 * Outputs:
 *  number of tests that failed in module
 */
int encodedSciDataTestMain() {
    int testsFailed = 0; // iterator to track how many tests have failed
    int testCase = 0;

    while(true) { // loop through all buffer cases

        // generate buffer
        uint16_t * buffer = generateBuffer(testCase);
        if (buffer == nullptr) { break; } // exit case

        //generate timestamp
        unsigned long timestamp = millis();
        unsigned long compareTimestamp = timestamp;

        // instantiate object of EncodedSciData class with test buffer and timestamp
        EncodedSciData encodedSciData(buffer, timestamp); 

        for (int i = 0; i < 10; i++) {
            // inject errors
            encodedSciData.injectError(i,i);
        }

        // scrub the data 
        encodedSciData.scrub();

        // compare new buffer and original buffer (should be equal)
        for (int i = 0; i<BUFFERSIZE; i++) {
            uint16_t *testBuffer = encodedSciData.getBuffer();
            if (buffer[i] != testBuffer[i]) {
                Serial.print("Buffer mismatch (test case ");
                Serial.print(testCase);
                Serial.print(", buffer index ");
                Serial.print(i);
                Serial.println(")");

                testsFailed += 1;
                break;
            }
        }

        // compare timestamp
        if (compareTimestamp != encodedSciData.getTimestamp()) {
            Serial.print("Timestamp mismatch (test case ");
            Serial.print(testCase);
            Serial.println(")");
            
            testsFailed += 1;
        }

        testCase++; // move to next test case
    }

    // print module summary
    Serial.print("Encoded Science Data module: ");
    Serial.print(testsFailed);
    Serial.println(" tests failed");

    return testsFailed;
}