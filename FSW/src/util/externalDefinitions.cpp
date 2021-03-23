/* externalDefinitions.cpp DEFINES all global objects declared with the 'extern' keyword in config.hpp
    The compiler will point all files to these definitions so that only one instance of each object exists globally.
    DO NOT DEFINE THESE OBJECTS ANYWHERE ELSE!!!
*/

// Config Header
#include "../headers/config.hpp"
#include "../headers/globalDeclarations.hpp"

/* GLOBAL DEFINITIONS */
// Main
Event exitMainLoopEvent = Event();

// Data Collection
volatile bool STREAM_PHOTO_DATA = STREAM_PHOTO_DATA_INIT;
RecurringEvent dataProcessEvent = RecurringEvent(SAMPLE_PERIOD_MSEC);
Event saveBufferEvent = Event();
TimedEvent sunriseTimerEvent = TimedEvent(WINDOW_LENGTH_MSEC);
TimedEvent sweepTimeoutEvent = TimedEvent(SWEEP_TIMEOUT_MSEC);
AsyncEvent downlinkEvent = AsyncEvent(MAXFILES);
AsyncEvent scrubEvent = AsyncEvent(MAXFILES);

// Fault Mitigation
volatile bool ACT_ON_NEW_FAULTS = ACT_ON_NEW_FAULTS_INIT;

// Housekeeping
volatile bool FORCE_HEATER_ON = FORCE_HEATER_ON_INIT;
volatile float OPTICS_THERM_CAL_VOLTAGE = OPTICS_THERM_CAL_VOLTAGE_INIT;
RecurringEvent housekeepingTimer = RecurringEvent(HK_SAMPLE_PERIOD_MSEC);

// Timing
ScienceMode scienceMode = ScienceMode();

// System Manager objects
CommandHandler commandHandler = CommandHandler();