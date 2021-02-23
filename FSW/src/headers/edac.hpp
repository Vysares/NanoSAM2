#ifndef EDAC_H
#define EDAC_H 

/* - - - - - - Includes - - - - - - */
// C++ libraries
#include <bits/stdc++.h> 

// Other libraries
#include <SerialFlash.h> // for file I/O to flash modules
#include <SPI.h> // will probably use this shortly, but it is not in use yet

// NS2 headers
#include "config.hpp"

/* - - - - - - Declarations - - - - - - */
typedef std::bitset<DATA_BLOCK_SIZE> hammingBlock_t;

#endif