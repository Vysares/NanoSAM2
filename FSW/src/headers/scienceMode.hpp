#ifndef TIMINGCLASS_H
#define TIMINGCLASS_H 

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries

// NS2 headers

/* - - - - - - Class Definitions - - - - - - */
class ScienceMode {
    protected:
        int mode; // according to mode enum
        bool adcsPointingAtSun; // flag from ADCS signaling optic is pointing at sun
        
    public:
        ScienceMode();
        int getMode();
        void setMode(int newMode);

        // setter and getter for adcsPointingAtSun
        bool getPointingAtSun();
        void setPointingAtSun(bool newState);
        void sweepChange();

        // FUTURE TEAMS: this event is invoked when the ADCS should switch its sweep direction
        //   so link your ADCS module with this event to tell it when to switch direction 
        //      look at checkSweepChange() in timing.cpp for more info
        Event sweepChangeEvent;
        
        // lockout to prevent sweep change while ADCS is reversing direction
        // (has placeholder duration for compilation, this is set properly during initialization)
        TimedEvent sweepChangeLockout; 

        // event indicating entry into standby mode
        Event onStandbyEntry;
};

#endif