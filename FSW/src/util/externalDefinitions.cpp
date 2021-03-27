/* externalDefinitions.cpp DEFINES all global objects declared with the 'extern' keyword in config.hpp
    The compiler will point all files to these definitions so that only one instance of each object exists globally.
    DO NOT DEFINE THESE OBJECTS ANYWHERE ELSE!!!
*/

// Config Header
#include "../headers/config.hpp"

/* GLOBAL DEFINITIONS */
// Main
Event exitMainLoopEvent = Event();

// Data Collection
volatile bool STREAM_PHOTO_SPI = STREAM_PHOTO_SPI_INIT;
volatile bool STREAM_PHOTO_DIRECT = STREAM_PHOTO_DIRECT_INIT;
RecurringEvent dataProcessEvent = RecurringEvent(SAMPLE_PERIOD_MSEC);
Event saveBufferEvent = Event();
TimedEvent sunriseTimerEvent = TimedEvent(WINDOW_LENGTH_MSEC);
TimedEvent sweepTimeoutEvent = TimedEvent(SWEEP_TIMEOUT_MSEC);
AsyncEvent downlinkEvent = AsyncEvent(MAXFILES);
AsyncEvent scrubEvent = AsyncEvent(MAXFILES);

// Command Handling
volatile bool DANGER_COMMANDS_ALLOWED = DANGER_COMMANDS_ALLOWED_INIT;

// Fault Mitigation
volatile bool SUPPRESS_FAULTS = SUPPRESS_FAULTS_INIT;
volatile bool ACT_ON_NEW_FAULTS = ACT_ON_NEW_FAULTS_INIT;
RecurringEvent wdTimer = RecurringEvent(WD_RESET_INTERVAL_MSEC);

// Housekeeping
volatile bool HEATER_ON = HEATER_ON_INIT;
volatile bool HEATER_OVERRIDE = HEATER_OVERRIDE_INIT;
volatile bool STREAM_TEMPERATURE = STREAM_TEMPERATURE_INIT;
volatile float OPTICS_THERM_CAL_VOLTAGE = OPTICS_THERM_CAL_VOLTAGE_INIT;
RecurringEvent housekeepingTimer = RecurringEvent(HK_SAMPLE_PERIOD_MSEC);

// Timing
ScienceMode scienceMode = ScienceMode();