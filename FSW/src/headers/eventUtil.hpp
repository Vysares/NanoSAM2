#ifndef EVENTUTIL_H
#define EVENTUTIL_H 

/* - - - - - - Includes - - - - - - */
// C++ libraries
#include <vector>

// Other libraries
#include <Arduino.h>

// NS2 headers


/* - - - - - - Declarations - - - - - - */

/* - - - - - - Class Definitions - - - - - - */

/* - - - - - - Event - - - - - - *
 * Usage:
 * Events are used for communication between modules without direct function calls.
 *  -To invoke an event call invoke().
 *  -To check if an event is invoked call checkInvoked().
 *      If the event is invoked, checkInvoked() will return true and de-invoke the event, 
 *      so it is ready to be invoked again.
 */
class Event {
    protected:
        bool isInvoked;
        
    public:
        Event();

        bool checkInvoked();
        void invoke();
};

/* - - - - - - TimedEvent - - - - - - *
 * Usage:
 * A TimedEvent is invoked automatically after a set duration.
 *  -To start the TimedEvent, call start().
 *  -To change the duration of a TimedEvent call setDuration(newDuration).
 *  -To check if the TimedEvent is invoked call checkInvoked().
 *      If the time has elapsed, checkInvoked() will return true and invoke the event.
 *      checkInvoked() will only return true one time. Call start() again to restart the timer.
 */
class TimedEvent : public Event {
    protected:
        unsigned long duration;
        unsigned long currentMillis;
        unsigned long nextInvokeMillis;

    public:
        TimedEvent(unsigned long newDuration);
        TimedEvent(); // overloaded constructor to set default duration

        void setDuration(unsigned long newDuration);
        bool checkInvoked();
        void start();
};

/* - - - - - - RecurringEvent - - - - - - *
 * Usage:
 * A RecurringEvent is a TimedEvent that automatically restarts when invoked.
 *  -To start the RecurringEvent, call start().
 *  -To change the duration of a RecurringEvent call setDuration(newDuration).
 *  -To check if the RecurringEvent is invoked call checkInvoked().
 *      If the time has elapsed, checkInvoked() will return true and automatically restart itself.
 */
class RecurringEvent : public TimedEvent {
    public:
        RecurringEvent(unsigned long newDuration);

        bool checkInvoked();
};

/* - - - - - - AsyncEvent - - - - - - *
 * Usage:
 * An AsyncEvent remains invoked for a set number of calls to checkInvoked() or until stop() is called
 * Use an AsyncEvent when you want to stagger a task over multiple loop iterations.
 *  -To invoke the AsyncEvent call invoke()
 *  -To change the maximum number of iterations call setMaxIter(). 
 *  -To get the current iteration call iter().
 *  -To de-invoke the AsyncEvent and reset the iteration count call stop().
 *  -To de-invoke the AsyncEvent without resetting the iteration count, call pause().
 *  -To check if the AsyncEvent is invoked call checkInvoked().
 *      once the AsyncEvent is invoked, checkInvoked() will return true and increment iteration
 *      until max iterations is reached or stop() is called.
 *      If max iterations is not specified, the AsyncEvent will stay invoked until stop() is called.
 */
class AsyncEvent : public Event {
    protected:
        int iteration;
        int maxIterations;

    public:
        AsyncEvent();
        AsyncEvent(int maxIter);

        void setMaxIter(int maxIter);
        void invoke();
        bool checkInvoked();
        int iter();
        void stop();
        void pause();
        bool first();
        bool over();
};

#endif