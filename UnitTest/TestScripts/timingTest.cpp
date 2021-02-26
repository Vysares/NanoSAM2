/* unitTestDriver.cpp is the main script for the NanoSAM II Unit Testing 
 * Usage:
 *  part of NS2 unit testing suite
 *  to be called in unitTestDriver.cpp
 *
 */


// NS2 includes
#include "../headers/config.hpp"
//#include "../headers/timingClass.hpp"
#include "../headers/timing.hpp"

/* - - - - - - testWrapBufferIdx - - - - - - *
 * Usage:
 * tests wrapBufferIdx with variety of idx
 * 
 * Inputs:
 *  none
 *  
 * Outputs:
 *  0 - pass
 *  1 - fail
 */
int testWrapBufferIdx() {

    // build test cases and answers
    int key[] = {0, BUFFERSIZE-1, BUFFERSIZE-1, 0, 1, 0};
    int testVal[] = {0, -1, BUFFERSIZE-1, BUFFERSIZE, 2*BUFFERSIZE + 1, -BUFFERSIZE};

    // call function to be tested
    for (int i=0; i<6; i++) {
        if (key[i] != wrapBufferIdx(testVal[i])) {
            Serial.println("wrapBufferIdx() unit test failed (timing module)");
            return 1;
        }
    }

    return 0;
}


/* - - - - - - testVoltageRunningMean - - - - - - *
 * Usage:
 * tests voltageRunningMean with variety of buffers
 * 
 * Inputs:
 *  none
 *  
 * Outputs:
 *  0 - pass
 *  1 - fail
 */
int testVoltageRunningMean() {

    // build test cases and answers
    float key[] = {0.200F, 0, 4.000F, 0, 16.000F};
    int testIdx[] = {0, BUFFERSIZE-2*SMOOTH_IDX_COUNT, 2*SMOOTH_IDX_COUNT, 
                      BUFFERSIZE-2*SMOOTH_IDX_COUNT, 2*SMOOTH_IDX_COUNT};
    float testBuf[BUFFERSIZE];

    for (int i = 0; i<BUFFERSIZE; i++) {
        if (i < BUFFERSIZE/2) {
            testBuf[i] = 1.000f; // set even idx to 2
        } else {
            testBuf[i] = 0;
        }
    }

    // call function to be tested
    for (int i = 0; i<5; i++) {
        if (key[i] != voltageRunningMean(testBuf, testIdx[i])){
            Serial.println("voltageRunningMean() unit test failed (timing module)");
            return 1;
        }
        
        // double values in buffer for next test
        for (int j = 0; j < BUFFERSIZE; j++) {
            testBuf[j] = testBuf[j] * 2;
        }
    }

    return 0;
}


/* - - - - - - timingTestMain - - - - - - *
 * Usage:
 * runs the timing module unit tests, prints results over serial
 *  must be kept last in file since we are not using header structure for testing
 *  (or if you would rather use prototypes be my guest)
 * 
 * Inputs:
 *  none
 *  
 * Outputs:
 *  number of tests that failed in module
 */
int timingTestMain(){
    int testsFailed = 0; // iterator to track how many tests have failed

    testsFailed += testWrapBufferIdx();
    testsFailed += testVoltageRunningMean();

    // print module summary
    Serial.print("Timing module: ");
    Serial.print(testsFailed);
    Serial.println(" tests failed");

    return testsFailed;
}