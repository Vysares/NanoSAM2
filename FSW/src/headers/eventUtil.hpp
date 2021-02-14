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

class Event
{
    protected:
        bool isInvoked;
        
    public:
        Event();
        bool checkInvoked();
        void invoke();
};


class TimedEvent : public Event
{
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

class RecurringEvent : public TimedEvent
{
    public:
        RecurringEvent(unsigned long newDuration);
        bool checkInvoked();
};

#endif