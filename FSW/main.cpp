/* main.cpp is the main script for the NanoSAM II FSW 
 * Usage:
 *  run this script through Teensyduino to power the NS2 payload
 * 
 * Additional files needed for compilation:
 *  memUtil.hpp
 *  memUtil.cpp
 *
 */

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries

// NS2 headers
#include "src/headers/memUtil.hpp"


/* - - - - - - Functions - - - - - - */

/* - - - - - - main - - - - - - *
 * Usage:
 *  Main function entered on runtime, fill with NS2 functionality
 * 
 * Inputs:
 *  none
 *  
 * Outputs:
 *  None
 */
int main()
{
    //carry out initializations
    
    // TODO: Change main loop to account for critical exit conditions
    while(true) // run main loop forever
    {
        // test multiple file structure by calling functions from memUtil.cpp
        scienceMemoryHandling();
        break;
    }

    return 0;
}

