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

    // call function to be tested

    int key[] = {0, BUFFERSIZE-1, BUFFERSIZE-1, 0, 1, 0};
    int testVal[] = {0, -1, BUFFERSIZE-1, BUFFERSIZE, 2*BUFFERSIZE + 1, -BUFFERSIZE};

    for (int i=0; i<6; i++) {
        if (key[i] != wrapBufferIdx(testVal[i])) {
            Serial.println("wrapBufferIdx() Unit Test Failed (timing module)");
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

    // call function to be tested

    return 1;
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

    return testsFailed;
}