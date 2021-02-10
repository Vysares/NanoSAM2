#ifndef CONFIG_H
#define CONFIG_H 
/* config.hpp contains mission-wide constants
 * Usage:
 *  define any mission-wide constants (pin numbers, 
 *  sampling rates, etc) that we will want to be able to change
 *  from a single location and include in multiple modules
 */

// NS2 Headers
#include "eventUtil.hpp"
#include "timing.hpp"

/* = = = = = = = = = = = = = = = = = = = = = = = = = =
 * = = = = = = Configuration Declarations  = = = = = = 
 * = = = = = = = = = = = = = = = = = = = = = = = = = */

/* - - - - - - Teensy Pins - - - - - - */
const int PIN_HEAT = 0;         // activate heater pin
const int PIN_WD_RESET = 2;     // Watchdog reset pin
const int PIN_ADC_CS = 10;      // ADC Chip Select Pin
const int PIN_AREG_CURR = 17;   // analog regulator current pin
const int PIN_DREG_CURR = 18;   // digital regulator current pin
const int PIN_DREG_PG = 20;     // digital regulator 'power good' pin
const int PIN_PHOTO = 21;       // direct photodiode data pin for use with Teensy ADC

/* - - - - - - SPI - - - - - - */
const int SPI_MAX_SPEED = 2000000; // clock speed of 2MHz for SPI

/* - - - - - - Serial - - - - - - */
const int SERIAL_BAUD = 9600;
const int SERIAL_TIMEOUT = 50;      // time to wait for serial input, ms

/* - - - - - - ADCS - - - - - - */
// NanoSAM II will assume that ADCS will be implemented later, but have places in the logic
// for a future team to slot in these ADCS measurements 
const bool ADCS_READY_FOR_SCIENCE = true;     // flag on whether or not attitude is ready for science

/* = = = = = = = = = = = = = = = = = = = = = =
 * = = = = = = Module Constants  = = = = = = =
 * = = = = = = = = = = = = = = = = = = = = = */

/* - - - - - - Memory Handling Module - - - - - - */
const int SAMPLING_RATE = 50;       // Hz, desired irradiance sampling rate
const int WINDOW_LENGTH_SEC = 240;  // seconds, length of science data 
const int MAXFILES = 10;            // maximum number of files in flash storage
const float ADC_BINS = 65536;       // bins, number of bins in ADC (2^16)
const float ADC_MAX_VOLTAGE = 3.3;  // Volts, upper end of ADC voltage range
const float ADC_MIN_VOLTAGE = 0.0;  // Volts, lower end of ADC voltage range

// TODO: Update this with size of actual timestamp once it is known
const int TIMESTAMP_SIZE = 1;   // array indices needed to store timestamp

// set number of measurements to store in science data buffer
const int BUFFERSIZE = SAMPLING_RATE * WINDOW_LENGTH_SEC; // indices
const int FILESIZE = BUFFERSIZE + TIMESTAMP_SIZE;

// timing constants
const unsigned long SAMPLE_PERIOD_MSEC = 1000 / (unsigned long)SAMPLING_RATE; // millisec, time between samples  
const int WINDOW_LENGTH_MSEC = WINDOW_LENGTH_SEC * 1000; // milliseconds, length of science data
const int SWEEP_TIMEOUT_MSEC = 1000; // milliseconds, time for ADCS to sweep optic across the sun

// Events
static RecurringEvent dataProcessEvent(SAMPLE_PERIOD_MSEC); // assuming that duration arg is ms
static Event saveBufferEvent;
static TimedEvent sunriseTimerEvent(WINDOW_LENGTH_MSEC);
static TimedEvent sweepTimeoutEvent(SWEEP_TIMEOUT_MSEC);

// FUTURE TEAMS: this event is invoked when the ADCS should switch its sweep direction
//   so link your ADCS module with this event to tell it when to switch direction 
//      look at checkSweepChange() in timing.cpp for more info
static Event sweepDirectionChangeEvent; 

/* - - - - - - Command Handling Module - - - - - - */
const int COMMAND_QUEUE_SIZE = 100;     // maximum number of commands the command queue can store.
const int SERIAL_TIMEOUT_MSEC = 50;     // milliseconds, maximum time to wait for serial input

/* - - - - - - Fault Mitigation Module - - - - - - */
const int WD_RESET_INTERVAL = 100;  // watchdog feeding interval, ms
const int WD_PULSE_DUR = 10;        // watchdog reset signal duration, MICROSECONDS!!!

/* - - - - - - Timing Module - - - - - - */
const float SUN_THRESH_VOLTAGE = (ADC_MAX_VOLTAGE - ADC_MIN_VOLTAGE) / 4; // value signifying we are pointing at sun
const int SMOOTH_IDX_COUNT = 5; // number of indices to use in smoothing the voltage buffer for mode change comparisons
const int ADCS_SWEEP_IDX_OFFSET = SMOOTH_IDX_COUNT; // number of indices to traverse backwards in buffer when checking ADCS sweep direction 

// timing science mode object declaration
ScienceMode scienceMode;

#endif