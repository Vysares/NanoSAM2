#ifndef TIMINGDEC_H
#define TIMINGDEC_H 
/* Function Declarations for Timing module
 * (class definitions must be included with higher 
 *   priority than config.hpp, but this file must be 
 *   included after config.hpp to use BUFFERSIZE) */

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries

// NS2 headers
#include "config.hpp"

/* - - - - - - Declarations - - - - - - */
void updatePayloadMode(float buffer[BUFFERSIZE], int bufIdx);
float smoothBuffer(float buffer[BUFFERSIZE], int bufIdx);
void checkSweepChange(float buffer[BUFFERSIZE], int bufIdx);
int wrapBufferIdx(int idx);
// TODO recoverPayloadMode();
#endif