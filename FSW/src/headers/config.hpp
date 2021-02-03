#ifndef CONFIG_H
#define CONFIG_H 
/* config.hpp is a header file 
 * Usage:
 *  define any mission-wide constants (pin numbers, 
 *  sampling rates, etc) that we will want to be able to change
 *  from a single location and include in multiple modules
 */

/* - - - - - - Declarations - - - - - - */
float dataProcessing();
void scienceMemoryHandling();
void updateBuffer(float sample, int &index);
bool saveBuffer(int &index);

#endif