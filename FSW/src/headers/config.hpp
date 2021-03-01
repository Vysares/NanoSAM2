#ifndef CONFIG_H
#define CONFIG_H 
/* config.hpp contains mission-wide constants
 * Usage:
 *  define any mission-wide constants (pin numbers, 
 *  sampling rates, etc) that we will want to be able to change
 *  from a single location and include in multiple modules
 * 
 *  declare all objects as static to avoid the error 
 *      "multiple definitions of <object>" during compilation
 */

//C++ libraries
#include <Arduino.h>
#include <cstdint>

// NS2 Headers
#include "eventUtil.hpp"
#include "timingClass.hpp"

/* = = = = = = = = = = = = = = = = = = = = = = = = = =
 * = = = = = = Configuration Declarations  = = = = = = 
 * = = = = = = = = = = = = = = = = = = = = = = = = = */

/* - - - - - - Teensy Pins - - - - - - */
const int PIN_HEAT = 0;             // activate heater pin
const int PIN_WD_RESET = 2;         // Watchdog reset pin
const int PIN_ADC_CS = 10;          // ADC Chip Select Pin
const int PIN_AREG_CURR = 17;       // analog regulator current pin
const int PIN_DREG_CURR = 18;       // digital regulator current pin
const int PIN_DREG_PG = 20;         // digital regulator 'power good' pin
const int PIN_PHOTO = 21;           // direct photodiode data pin for use with Teensy ADC
const int PIN_DIGITAL_THERM = 14;   // digital board thermistor pin
const int PIN_ANALOG_THERM = 15;    // analog board thermistor pin
const int PIN_OPTICS_THERM = 16;    // optics bench thermistor pin


/* - - - - - - Teensy ADC - - - - - - */
const int TEENSY_ADC_BINS = 1023;       // bins, number of bins in Teensy ADC
const float TEENSY_HIGH_VOLTAGE = 3.3;  // volts, max teensy voltage
const float TEENSY_LOW_VOLTAGE = 0.0;   // volts, min teensy voltage
const float TEENSY_VOLTAGE_RES = (TEENSY_HIGH_VOLTAGE - TEENSY_LOW_VOLTAGE) / TEENSY_ADC_BINS; // volts per Teensy ADC bin

/* - - - - - - SPI - - - - - - */
const int ADC_MAX_SPEED = 2000000; // Hz, maximum SPI clock speed for ADC

/* - - - - - - Serial - - - - - - */
const int SERIAL_BAUD = 9600;       // Hz, baud rate of serial connection
const int SERIAL_TIMEOUT_MSEC = 50; // milliseconds, time to wait for serial input

/* - - - - - - ADCS - - - - - - */
// NanoSAM II will assume that ADCS will be implemented later, but have places in the logic
// for a future team to slot in these ADCS measurements 
const bool ADCS_READY_FOR_SCIENCE = true;     // flag on whether or not attitude is ready for science

/* = = = = = = = = = = = = = = = = = = = = = =
 * = = = = = = Module Constants  = = = = = = =
 * = = = = = = = = = = = = = = = = = = = = = */

/* - - - - - - Error Detection and Correction (EDAC) Module - - - - - - */
// do not change these.
const int HAMMING_BLOCK_SIZE = 9;  // bytes of data in a block, including parity bits
const int MESSAGE_SIZE = 8;        // bytes of non-redundant data in a block


/* - - - - - - Data Collection Module - - - - - - */
const int SAMPLING_RATE = 50;       // Hz, desired irradiance sampling rate
const int WINDOW_LENGTH_SEC = 240;  // seconds, length of science data 
const int MAXFILES = 10;            // maximum number of files in flash storage
const float ADC_BINS = 65536;       // bins, number of bins in ADC (2^16)
const float ADC_MAX_VOLTAGE = 3.3;  // Volts, upper end of ADC voltage range
const float ADC_MIN_VOLTAGE = 0.0;  // Volts, lower end of ADC voltage range
const float ADC_VOLTAGE_RES = (ADC_MAX_VOLTAGE - ADC_MIN_VOLTAGE) / ADC_BINS; // volts per ADC bin

// TODO: Update this with size of actual timestamp once it is known
const int TIMESTAMP_SIZE = sizeof(unsigned long);   // bytes needed to store timestamp

// data size parameters, derived from other constants
const int BUFFERSIZE = SAMPLING_RATE * WINDOW_LENGTH_SEC; // number of samples to keep in science buffer
const int BUFFER_MEMSIZE = BUFFERSIZE * sizeof(uint16_t); // bytes, size of data buffer
const int RAW_DATA_SIZE = BUFFER_MEMSIZE + TIMESTAMP_SIZE; // bytes, combined size of science data
const int MESSAGE_COUNT = (RAW_DATA_SIZE / MESSAGE_SIZE) + !!(RAW_DATA_SIZE % MESSAGE_SIZE); // number of message blocks per file
const int DECODED_FILE_SIZE = MESSAGE_COUNT * MESSAGE_SIZE; // bytes, size of all unencoded messages
const int ENCODED_FILE_SIZE = MESSAGE_COUNT * HAMMING_BLOCK_SIZE; // bytes, size of encoded data in file

// timing constants
const unsigned long SAMPLE_PERIOD_MSEC = 1000 / (unsigned long)SAMPLING_RATE; // milliseconds, time between samples  
const int WINDOW_LENGTH_MSEC = WINDOW_LENGTH_SEC * 1000; // milliseconds, length of science data
const int SWEEP_TIMEOUT_MSEC = 1000; // milliseconds, time for ADCS to sweep optic across the sun

// Events
static RecurringEvent dataProcessEvent(SAMPLE_PERIOD_MSEC); // assuming that duration arg is ms
static Event saveBufferEvent;
static TimedEvent sunriseTimerEvent(WINDOW_LENGTH_MSEC);
static TimedEvent sweepTimeoutEvent(SWEEP_TIMEOUT_MSEC);
static AsyncEvent scrubEvent(MAXFILES);


/* - - - - - - Command Handling Module - - - - - - */
const int COMMAND_QUEUE_SIZE = 100;     // maximum number of commands the command queue can store.


/* - - - - - - Fault Mitigation Module - - - - - - */
const int WD_RESET_INTERVAL_MSEC = 100;     // milliseconds, watchdog feeding interval
const int WD_PULSE_DUR_MICROSEC = 10;       // microseconds, watchdog reset signal duration


/* - - - - - - Housekeeping Module - - - - - - */
const int HK_SAMPLES_TO_KEEP = 5000;   // number of previous housekeeping samples to keep

// heater cutoff temperatures
static bool FORCE_HEATER_ON = false;   // if true, heater will always be on regardless of temperature
const float HEATER_TEMP_LOW = -20;   // celsius, heater will turn on at or below this temp
const float HEATER_TEMP_HIGH = 20;   // celsius, heater will turn off at or above this temp

// optics thermistor calibration
const float OPTICS_THERM_CAL_TEMP = 30;            // celsius, known temperature of optics baseline
static float OPTICS_THERM_CAL_VOLTAGE = 1.777F;    // volts, thermistor voltage at baseline temp
const float OPTICS_THERM_GAIN = -0.0109;           // volts/deg celsius, V/T relationship for optics thermistor

// safe temperature range
const float OPTICS_TEMP_MIN_SAFE = -35; // celsius, minimum safe photodiode temp
const float OPTICS_TEMP_MAX_SAFE = 70;  // celsius, maximum safe photodiode temp
const float BOARD_TEMP_MIN_SAFE = -20;    // celsius, minimum safe board temp
const float BOARD_TEMP_MAX_SAFE = 70;   // celsius, maximum safe board temp

// power supply expected voltage range
const float PG_VOLTAGE_MAX_EXPECTED = 3.4;  // volts, maximum expected reading from "power good" pin
const float PG_VOLTAGE_MIN_EXPECTED = 3.2;  // volts, minimum expected reading from "power good" pin

// timing constants
const int HK_SAMPLE_PERIOD_MSEC = 1000;    // milliseconds, interval between housekeeping updates

// Events
static RecurringEvent housekeepingTimer(HK_SAMPLE_PERIOD_MSEC);


/* - - - - - - Timing Module - - - - - - */
const float SUN_THRESH_VOLTAGE = (ADC_MAX_VOLTAGE - ADC_MIN_VOLTAGE) / 4; // value signifying we are pointing at sun
const int SMOOTH_IDX_COUNT = 5; // number of indices to use in smoothing the voltage buffer for mode change comparisons
const int ADCS_SWEEP_IDX_OFFSET = SMOOTH_IDX_COUNT; // number of indices to traverse backwards in buffer when checking ADCS sweep direction 
const int ADCS_SWEEP_CHANGE_DURATION = 2 * SMOOTH_IDX_COUNT * SAMPLE_PERIOD_MSEC; // millisec, duration to prevent ADCS sweep direction change

// timing science mode object declaration
static ScienceMode scienceMode;

#endif
