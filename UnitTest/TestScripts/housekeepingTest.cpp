/* commandTest.cpp tests the command module non-void functions
 * Usage:
 *  part of NS2 unit testing suite
 *  to be called in unitTestDriver.cpp
 *
 */


// NS2 includes
#include "../headers/config.hpp"
#include "../headers/housekeeping.hpp"

/* - - - - - - testVoltageToBoardTemp - - - - - - *
 * Usage:
 * tests voltageToBoardTemp with all possible commands
 * 
 * Inputs:
 *  none
 *  
 * Outputs:
 *  0 - pass
 *  1 - fail
 */
int testVoltageToBoardTemp() {
    //TODO
}

/* - - - - - - testVoltageToBoardTemp - - - - - - *
 * Usage:
 * tests voltageToBoardTemp with all possible commands
 * 
 * Inputs:
 *  none
 *  
 * Outputs:
 *  0 - pass
 *  1 - fail
 */
int testVoltageToOpticsTemp() {
    //TODO
}


/* - - - - - -  housekeepingTestMain - - - - - - *
 * Usage:
 * runs the command module unit tests, prints results over serial
 *  must be kept last in file since we are not using header structure for testing
 *  (or if you would rather use prototypes be my guest)
 * 
 * Inputs:
 *  none
 *  
 * Outputs:
 *  number of tests that failed in module
 */
int housekeepingTestMain() {
    int testsFailed = 0; // iterator to track how many tests have failed

    testsFailed += testVoltageToBoardTemp();
    testsFailed += testVoltageToOpticsTemp();

    // print module summary
    Serial.print("Housekeeping module: ");
    Serial.print(testsFailed);
    Serial.println(" tests failed");

    return testsFailed;
}