#ifndef CONFIG_H
#define CONFIG_H 
/* config.hpp contains mission-wide constants
 * Usage:
 *  define any mission-wide constants (pin numbers, 
 *  sampling rates, etc) that we will want to be able to change
 *  from a single location and include in multiple modules
 */

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

/* = = = = = = = = = = = = = = = = = = = = = =
 * = = = = = = Module Constants  = = = = = = =
 * = = = = = = = = = = = = = = = = = = = = = */

/* - - - - - - Memory Handling Module - - - - - - */
const int SAMPLING_RATE = 50;   // Hz, desired irradiance sampling rate
const int WINDOW_LENGTH = 240;  // seconds, length of science data buffer 
const int MAXFILES = 10;        // maximum number of files in flash storage
const float ADC_BINS = 65536;       // bins, number of bins in ADC (2^16)
const float ADC_MAX_VOLTAGE = 3.3;  // Volts, upper end of ADC voltage range
const float ADC_MIN_VOLTAGE = 0.0;  // Volts, lower end of ADC voltage range

// TODO: Update this with size of actual timestamp once it is known
const int TIMESTAMP_SIZE = 1;   // array indices needed to store timestamp

// set number of measurements to store in science data buffer
const int BUFFERSIZE = SAMPLING_RATE * WINDOW_LENGTH; // indices
const int FILESIZE = BUFFERSIZE + TIMESTAMP_SIZE;

/* - - - - - - Fault Mitigation Module - - - - - - */
const int WD_RESET_INTERVAL = 100;  // watchdog feeding interval, ms
const int WD_PULSE_DUR = 10;        // watchdog reset signal duration, MICROSECONDS!!!

#endif