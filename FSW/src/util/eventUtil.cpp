/* eventUtil.cpp contains the Event class and TimerPeriodic class 
 * Usage:
 *  Events are used to communicate between modules without direct function references
 *  
 *
 * Additional files needed for compilation:
 *  eventUtil.hpp
 */

/* - - - - - - Includes - - - - - - */
// All libraries are put in eventUtil.hpp
// NS2 headers
#include "../headers/eventUtil.hpp"

/* - - - - - - Class Implementations - - - - - - */

/* - - - - - - Event - - - - - - *
 * Usage:
 * Events are used for communication between modules without direct function calls.
 *  To invoke an event call invoke().
 *  To check if an event is invoked call checkInvoked().
 *      If the event is invoked, checkInvoked() will return true and de-invoke the event, 
 *      so it is ready to be invoked again.
 */
Event::Event() { // constructor
    isInvoked = false;
}

bool Event::checkInvoked() {
    // will only return true once
    bool returnVal = isInvoked;
    isInvoked = false; // reset invoked status
    return returnVal;
}

void Event::invoke() {
    isInvoked = true;
}


/* - - - - - - TimedEvent - - - - - - *
 * Usage:
 * A TimedEvent is invoked automatically after a set duration.
 *  To start the TimedEvent, call start().
 *  To change the duration of a TimedEvent call setDuration(newDuration).
 *  To check if the TimedEvent is invoked call checkInvoked().
 *      If the time has elapsed, checkInvoked() will return true and invoke the event.
 *      checkInvoked() will only return true one time. Call start() again to restart the timer.
 */
TimedEvent::TimedEvent(unsigned long newDuration) { // constructor
    duration = newDuration; 
    currentMillis = millis();
    nextInvokeMillis = 0;
    isInvoked = true;  // A timed event will not invoke itself unless start() is first called
}

TimedEvent::TimedEvent() { // overloaded constructor to set default duration
    unsigned long defaultDuration = 1000; // milliseconds
    duration = defaultDuration; 
    currentMillis = millis();
    nextInvokeMillis = 0;
    isInvoked = true;  // A timed event will not invoke itself unless start() is first called
}

void TimedEvent::setDuration(unsigned long newDuration) { // sets new duration
    duration = newDuration;
}

bool TimedEvent::checkInvoked() {
    // will only return true once
    if (!isInvoked && (millis() >= nextInvokeMillis)) {
        this->invoke();
        return true;
    }
    return false;
}

void TimedEvent::start() { // starts the timed event
    isInvoked = false;
    nextInvokeMillis = millis() + duration;
}

/* - - - - - - RecurringEvent - - - - - - *
 * Usage:
 * A RecurringEvent is a TimedEvent that automatically restarts when invoked.
 *  To start the RecurringEvent, call start().
 *  To change the duration of a RecurringEvent call setDuration(newDuration).
 *  To check if the RecurringEvent is invoked call checkInvoked().
 *      If the time has elapsed, checkInvoked() will return true and automatically restart itself.
 */
RecurringEvent::RecurringEvent(unsigned long newDuration) : TimedEvent(newDuration) { }; // constructor

bool RecurringEvent::checkInvoked() {
    // will return true once and automatically restart
    if (!isInvoked && millis() >= nextInvokeMillis) {
        this->start();
        return true;
    }
    return false;
}
