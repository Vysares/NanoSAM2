#ifndef TIMINGCLASS_H
#define TIMINGCLASS_H 

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries

// NS2 headers

/* - - - - - - Class Definitions - - - - - - */
class ScienceMode
{
    protected:
        int mode; // according to mode enum
        // could add more states such as ADCS ready, sweep direction, etc.

    public:
        ScienceMode();
        int get();
        void set(int newMode);
};

#endif