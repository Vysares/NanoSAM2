/* commandTest.cpp tests the command module non-void functions
 * Usage:
 *  part of NS2 unit testing suite
 *  to be called in unitTestDriver.cpp
 *
 */


// NS2 includes
#include "../headers/config.hpp"
#include "../headers/commandHandling.hpp"

/* - - - - - - testCheckMetaCommand - - - - - - *
 * Usage:
 * tests checkMetaCommand with all possible commands
 * 
 * Inputs:
 *  none
 *  
 * Outputs:
 *  0 - pass
 *  1 - fail
 */
int testCheckMetaCommand() {
    
    bool key[] = {false, false, false, false, false,         // normal commands
                  false, false, false, false, false, false,  // normal commands
                  true, true, true,                          // meta commands
                  false, false, false, false, false};        // normal commands

    // iterate through all commands
    for (int i = 1; i <= 19; i++) { // Command enum is 1-indexed
        
        if (true) { // guard to enable/disable debug prints
            Serial.println("Debugging checkMetaCommand()")
            Serial.print("key[");
            Serial.print(i);
            Serial.print("] = ");
            Serial.print(key[i]);
            Serial.print("  -  checkMetaCommand(");
            Serial.print(i);
            Serial.print(") = ");
            Serial.println(checkMetaCommand(i));
        }
        
        // call function
        if (key[i] != checkMetaCommand(i)) {
            Serial.println("checkMetaCommand() unit test failed (command handling module)");
            return 1;
        }
    }
    
    return 0;
}


/* - - - - - - commandTestMain - - - - - - *
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
int commandTestMain() {
    int testsFailed = 0; // iterator to track how many tests have failed

    testsFailed += testCheckMetaCommand();

    // print module summary
    Serial.print("Command Handling module: ");
    Serial.print(testsFailed);
    Serial.println(" tests failed");

    return testsFailed;
}
