/* unitTestDriver.cpp is the main script for the NanoSAM II Unit Testing 
 * Usage:
 *  run this script through Teensyduino to execute NS2 unit testing suite
 *  see instructions.md in this directory for detailed instructions on compilation
 *
 */

/* function prototypes */
// since we are not using the header format for unit testing

int timingTestMain();


/* - - - - - - main - - - - - - *
 * Usage:
 * call each module unit test here
 *  comment/uncomment if you want to enable/disable specific tests
 * 
 * Inputs:
 *  none
 *  
 * Outputs:
 *  None
 */
int main(){

    int testFailCount = 0; // track how many total tests failed

    // testing functions 
    testFailCount += timingTestMain();


    // print summary of test results
    Serial.println("\n - - - - Unit Test Summary - - - - -");
    Serial.print("|     ");
    Serial.print(testFailCount);
    Serial.println(" test(s) failed in total     |");
    Serial.println(" - - - - - - - - - - - - - - - - - -");

    return 1;
}
