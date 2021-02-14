/* hkUtil.cpp handles NS2 payload housekeeping
 * Usage:
 *  module functionality
 *      system state data collection
 *      temperature control
 * 
 * Modules encompassed:
 *  houskeeping
 *  thermal control
 *
 * Additional files needed for compilation:
 *  none
 */

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries

// NS2 headers
#include "../headers/hkUtil.hpp"


/* Module Variable Definitions */
static HousekeepingData hkData[HK_SAMPLES_TO_KEEP] = {}; // array to store housekeeping data
static int hkIndex = 0;

/* - - - - - - Module Driver Functions - - - - - - */

/* - - - - - - handleHousekeeping - - - - - - *
 * Usage:
 *  Handles all housekeeping functions.
 *  Samples temperature and power data, handles thermal control
 *  
 * 
 * Inputs:
 *  none
 *  
 * Outputs:
 *  None
 */
void handleHousekeeping()
{
    sampleHousekeepingData();
    setHeater();
    hkIndex++;
    if (hkIndex >= HK_SAMPLES_TO_KEEP) { hkIndex = 0; }
}

/* - - - - - - Module Helper Functions - - - - - - */

/* - - - - - - setHeater - - - - - - *
 * Usage:
 *  Toggles heater based on current optics bench temperature
 *  Turns heater on when optics temp crosses below HEATER_TEMP_LOW
 *  Turns heater off when optics temp crosses above HEATER_TEMP_HIGH
 * 
 * Inputs:
 *  none
 *  
 * Outputs:
 *  None
 */
void setHeater()
{
    if (latestHkSample.opticsTemp >= HEATER_TEMP_HIGH)
    {
        digitalWrite(PIN_HEAT, LOW); // turn heater off
    }
    else if (latestHkSample.opticsTemp <= HEATER_TEMP_LOW)
    {
        digitalWrite(PIN_HEAT, HIGH); // turn heater on
    }
}

/* - - - - - - sampleHousekeepingData - - - - - - *
 * Usage:
 *  samples temperature and power data and stores it in hkData,
 *  checks for out-of-bounds values and logs faults if found
 * 
 * Inputs:
 *  none
 *  
 * Outputs:
 *  None
 */
void sampleHousekeepingData()
{
    // read thermistor voltage values
    float opticsTempVoltage = TEENSY_VOLTAGE_RES*analogRead(PIN_OPTICS_THERM);
    float analogTempVoltage = TEENSY_VOLTAGE_RES*analogRead(PIN_ANALOG_THERM);
    float digitalTempVoltage = TEENSY_VOLTAGE_RES*analogRead(PIN_DIGITAL_THERM);
    
    // update housekeeping data
    latestHkSample.opticsTemp = voltageToTemp(opticsTempVoltage); 
    latestHkSample.analogTemp = voltageToTemp(analogTempVoltage);
    latestHkSample.digitalTemp = voltageToTemp(digitalTempVoltage);
    latestHkSample.analogCurrent = TEENSY_VOLTAGE_RES*(float)analogRead(PIN_AREG_CURR);
    latestHkSample.digitalCurrent = TEENSY_VOLTAGE_RES*(float)analogRead(PIN_DREG_CURR);   
    latestHkSample.digitalRegPG = TEENSY_VOLTAGE_RES*(float)analogRead(PIN_DREG_PG);
    hkData[hkIndex] = latestHkSample; // update data array

    // Check if values are in acceptable ranges
    // TODO: Log a fault for every case
    if (latestHkSample.opticsTemp > OPTICS_TEMP_MAX_SAFE) { } // optics temp too high
    else if (latestHkSample.opticsTemp < OPTICS_TEMP_MIN_SAFE) { } // optics temp too low

    if (latestHkSample.analogTemp > BOARD_TEMP_MAX_SAFE) { } // analog board temp too high
    else if (latestHkSample.analogTemp < BOARD_TEMP_MIN_SAFE) { } // analog board temp too low

    if (latestHkSample.digitalTemp > BOARD_TEMP_MAX_SAFE) { } // digital board temp too high
    else if (latestHkSample.digitalTemp < BOARD_TEMP_MIN_SAFE) { } // digital board temp too low

    if (latestHkSample.digitalRegPG > PG_VOLTAGE_MAX_EXPECTED) { } // digital regulator PG signal too high
    else if (latestHkSample.digitalRegPG < PG_VOLTAGE_MIN_EXPECTED) { } // digital regulator PG signal too low

}

/* - - - - - - voltageToTemp - - - - - - *
 * Usage:
 *  Interpolates and returns the corresponding temperature for a given thermistor voltage reading
 *  based on the data in thermLookup. 
 *  Will extrapolate beyond max and min entries in thermLookup if needed
 * 
 * Inputs:
 *  voltage - thermistor voltage
 *  
 * Outputs:
 *  temperature - celsius, the temperature corresponding to thermistor voltage reading
 */
float voltageToTemp(float voltage)
{
    int thermLookupSize = sizeof(thermLookup) / sizeof(thermLookup[0]); // size of lookup table
    
    // find lower adjacent entry
    int i = 0;
    while (thermLookup[i].voltage > voltage && i < thermLookupSize - 1) { i++; }
    
    float tempGap = thermLookup[i - 1].temperature - thermLookup[i].temperature; // temp difference between upper and lower entries
    float voltageGap = thermLookup[i - 1].voltage - thermLookup[i].voltage; // voltage difference between upper and lower entries
    
    // interpolate temperature
    float temperature = tempGap*(voltage - thermLookup[i].voltage) / voltageGap + thermLookup[i].temperature;
    return temperature;
}

/* - - - - - - timeSortHkData - - - - - - *
 * Usage:
 *  Sorts hkData in ascending cronological order and resets hkIndex to 0
 *  call before using hkData in another function
 * 
 * Inputs:
 *  none
 *  
 * Outputs:
 *  none
 */
void timeSortHkData()
{
    // create array to hold data in time ascending order
    HousekeepingData timeSortHkData[HK_SAMPLES_TO_KEEP];
    int j = 0; // iterator for sorted array index

    // reorder array so that it is ascending in time
    for (int i = hkIndex; i < HK_SAMPLES_TO_KEEP; i++)
    {
        timeSortHkData[j] = hkData[i];
        j++;
    } 

    // loop back to top of array and store remaining values
    for (int i = 0; i < hkIndex; i++)
    {
        timeSortHkData[j] = hkData[i];
        j++;
    }

    // copy sorted array to hkData
    for (int i = 0; i < HK_SAMPLES_TO_KEEP; i++)
    {
        hkData[i] = timeSortHkData[i];
    }
    hkIndex = 0; // reset index
}
