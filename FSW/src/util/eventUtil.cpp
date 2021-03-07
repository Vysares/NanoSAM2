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

/* - - - - - - Class Definitions - - - - - - */

/* - - - - - - Event - - - - - - */
// constructor
Event::Event() {
    isInvoked = false;
}

// checks if the event is invoked
bool Event::checkInvoked() {
    // will only return true once
    bool returnVal = isInvoked;
    isInvoked = false; // reset invoked status
    return returnVal;
}

// invokes the event
void Event::invoke() {
    isInvoked = true;
}


/* - - - - - - TimedEvent - - - - - - */
// constructor
TimedEvent::TimedEvent(unsigned long newDuration) {
    duration = newDuration; 
    currentMillis = millis();
    nextInvokeMillis = 0;
    isInvoked = true;  // A timed event will not invoke itself unless start() is first called
}

// overloaded constructor to set default duration
TimedEvent::TimedEvent() {
    unsigned long defaultDuration = 1000; // milliseconds
    duration = defaultDuration; 
    currentMillis = millis();
    nextInvokeMillis = 0;
    isInvoked = true;  // A timed event will not invoke itself unless start() is first called
}

// sets new duration
void TimedEvent::setDuration(unsigned long newDuration) { 
    duration = newDuration;
}

// checks if the event is invoked
bool TimedEvent::checkInvoked() {
    // will only return true once
    if (!isInvoked && (millis() >= nextInvokeMillis)) {
        invoke();
        return true;
    }
    return false;
}

// starts the timed event
void TimedEvent::start() { 
    isInvoked = false;
    nextInvokeMillis = millis() + duration;
}


/* - - - - - - RecurringEvent - - - - - - */
// constructor
RecurringEvent::RecurringEvent(unsigned long newDuration) : TimedEvent(newDuration) { }; 

bool RecurringEvent::checkInvoked() {
    // will return true once and automatically restart
    if (!isInvoked && millis() >= nextInvokeMillis) {
        start();
        return true;
    }
    return false;
}


/* - - - - - - AsyncEvent - - - - - - */
 // default constructor
AsyncEvent::AsyncEvent() : Event() {
    maxIterations = -1; // will not trigger automatic stop
    iteration = 0;
}

// overloaded constructor to set max iterations
AsyncEvent::AsyncEvent(int maxIter) : Event() { 
    maxIterations = maxIter;
    iteration = 0;
}

// sets new max number of iterations
void AsyncEvent::setMaxIter(int maxIter) { 
    maxIterations = maxIter;
}

// invokes the event
void AsyncEvent::invoke() {
    isInvoked = true;
}

// checks if the event is invoked
bool AsyncEvent::checkInvoked() { 
    if (maxIterations > 0 && iteration >= maxIterations) {
        stop();
    } else if (isInvoked) {
        iteration++;
    }
    return isInvoked;
}

// returns the current iteration
int AsyncEvent::iter() {
    return iteration; // will return 1 after first call of checkInvoked()
}

// de-invokes the event and resets current iteration to 0
void AsyncEvent::stop() {
    isInvoked = false;
    iteration = 0;
}

// de-invokes the event, current iteration will carry over to next call of invoke()
void AsyncEvent::pause() {
    isInvoked = false;
}

// returns whether the current iteration is the first
bool AsyncEvent::first() {
    return (iteration == 1);
}

// returns whether the current iteration is the last
bool AsyncEvent::over() {
    return (iteration == maxIterations);
}