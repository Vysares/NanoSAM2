/* watchdog.cpp sends a signal to trigger the watchdog
 * Usage:
 *  run this script in sequence with "Electronics System Test" document
 *  in order to validate the watchdog works
 *
 */

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries

// NS2 headers

int dogPin = 2; 

void loop()
{
    while(true) // run main loop forever
    {
		// Send a 3.3V, 50ns width square wave to trigger dog
		squareWave = 0; // TOOD: Construct square wave
		digitalWrite(dogPin, squareWave);
		
		// After signal, the Teensy should have shut off
		// TODO: Some sort of reset function? Not sure how this works...
		shutDownHandling()
    }

    return 0;
}

