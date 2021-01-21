/* memUtil.cpp handles NS2 payload memory allocation and assignment 
 * Usage:
 *  module functionality
 *  function definitions
 *  put function declarations in memUtil.hpp
 * 
 * Modules encompassed:
 *  Data Processing
 *  Science Memory Handling
 *
 * Additional files needed for compilation:
 *  none
 */

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries

// NS2 headers
#include "../headers/memUtil.hpp"

/* - - - - - - Module Driver Functions - - - - - - */

/* - - - - - - dataProcessing - - - - - - *
 * Usage:
 *  Reads incoming data from ADC, measures against baseline,
 *  appends time and pointing data, sends to memory.
 * 
 * Inputs:
 *  none
 *  
 * Outputs:
 *  None
 */
void dataProcessing()
{
    std::cout << "Data Processing Module: Test Print" << std::endl;
}


/* - - - - - - scienceMemoryHandling - - - - - - *
 * Usage:
 *  Takes data from processing and stores it in a buffer,
 *  continuously overwriting old entries.
 *  Copies buffer to long term memory when signaled by timing module.
 * 
 * Inputs:
 *  none
 *  
 * Outputs:
 *  None
 */
void scienceMemoryHandling()
{
    std::cout << "Science Memory Handling Module: Test Print" << std::endl;
}


/* - - - - - - Helper Functions - - - - - - */
