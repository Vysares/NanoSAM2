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

// NS2 Headers
#include "timingClass.hpp"
#include "eventUtil.hpp"

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
const float TEENSY_VOLTAGE_RANGE = TEENSY_HIGH_VOLTAGE - TEENSY_LOW_VOLTAGE; // volts, Teensy ADC voltage range
const float TEENSY_VOLTAGE_RES = TEENSY_VOLTAGE_RANGE / TEENSY_ADC_BINS;     // volts per bin

/* - - - - - - SPI - - - - - - */
const int ADC_MAX_SPEED = 2000000; // Hz, maximum SPI clock speed for ADC

/* - - - - - - Serial - - - - - - */
const int SERIAL_BAUD = 9600;       // Hz, baud rate of serial connection
const int SERIAL_TIMEOUT = 50;      // milliseconds, time to wait for serial input

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
const unsigned long SAMPLE_PERIOD_MSEC = 1000 / (unsigned long)SAMPLING_RATE; // milliseconds, time between samples  
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
const int WD_RESET_INTERVAL_MSEC = 100;     // milliseconds, watchdog feeding interval
const int WD_PULSE_DUR_MICROSEC = 10;       // microseconds, watchdog reset signal duration


/* - - - - - - Housekeeping Module - - - - - - */
const int HK_SAMPLES_TO_KEEP = 20;   // number of previous housekeeping samples to keep

// heater cutoff temperatures
const float HEATER_TEMP_LOW = 0;     // celsius, heater will turn on at or below this temp
const float HEATER_TEMP_HIGH = 20;   // celsius, heater will turn off at or above this temp

// safe temperature range
const float MIN_SAFE_OPTICS_TEMP = -20; // celsius, minimum safe photodiode temp
const float MAX_SAFE_OPTICS_TEMP = 50;  // celsius, maximum safe photodiode temp
const float MIN_SAFE_BOARD_TEMP = 0;    // celsius, minimum safe board temp
const float MAX_SAFE_BOARD_TEMP = 50;   // celsius, maximum safe board temp

// thermistor constants
const float THERM_VCC = 3.3;           // volts


// timing constants
const int HK_SAMPLE_PERIOD_MSEC = 1000;    // milliseconds, interval between housekeeping updates

// Events
static RecurringEvent housekeepingEvent(HK_PERIOD_MSEC);


/* - - - - - - Timing Module - - - - - - */
const float SUN_THRESH_VOLTAGE = (ADC_MAX_VOLTAGE - ADC_MIN_VOLTAGE) / 4; // value signifying we are pointing at sun
const int SMOOTH_IDX_COUNT = 5; // number of indices to use in smoothing the voltage buffer for mode change comparisons
const int ADCS_SWEEP_IDX_OFFSET = SMOOTH_IDX_COUNT; // number of indices to traverse backwards in buffer when checking ADCS sweep direction 

// timing science mode object declaration
static ScienceMode scienceMode;

#endif