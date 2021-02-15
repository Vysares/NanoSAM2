/* eventUtil.cpp contains the Event class and TimerPeriodic class 
 * Usage:
 *  Events are used to communicate between modules without direct function references
 *  
 *
 * Additional files needed for compilation:
 *  eventUtil.hpp
 */

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries

// NS2 headers
#include "../headers/eventUtil.hpp"

/* - - - - - - Class Implementations - - - - - - */

/* - - - - - - DirectEvent - - - - - - *
 * Usage:
 * Events are used for communication between modules without direct function calls
 */
Event::Event() // constructor
{
    isInvoked = false;
}

bool Event::checkInvoked() 
{
    // will only return true once
    bool returnVal = isInvoked;
    isInvoked = false; // reset invoked status
    return returnVal;
}

void Event::invoke()
{
    isInvoked = true;
}


/* - - - - - - TimedEvent - - - - - - *
 * Usage:
 * An event that is automatically invoked after a set duration
 */
TimedEvent::TimedEvent(unsigned long newDuration) // constructor
{
    duration = newDuration; 
    currentMillis = millis();
    nextInvokeMillis = 0;
    isInvoked = true;  // A timed event will not invoke itself unless start() is first called
}

TimedEvent::TimedEvent() // overloaded constructor to set default duration
{
    unsigned long defaultDuration = 1000; // milliseconds
    duration = defaultDuration; 
    currentMillis = millis();
    nextInvokeMillis = 0;
    isInvoked = true;  // A timed event will not invoke itself unless start() is first called
}

void TimedEvent::setDuration(unsigned long newDuration) // set new duration
{
    duration = newDuration;
}

bool TimedEvent::checkInvoked()
{
    // will only return true once
    if (!isInvoked && (millis() >= nextInvokeMillis)) 
    {
        this->invoke();
        return true;
    }
    return false;
}

void TimedEvent::start()
{   
    isInvoked = false;
    nextInvokeMillis = millis() + duration;
}

/* - - - - - - RecurringEvent - - - - - - *
 * Usage:
 * An event that is repeatedly invoked on a set interval
 */
RecurringEvent::RecurringEvent(unsigned long newDuration) : TimedEvent(newDuration) { };

bool RecurringEvent::checkInvoked()
{
    // will return true once and automatically restart
    if (!isInvoked && millis() >= nextInvokeMillis)
    {
        this->start();
        return true;
    }
    return false;
}
