/* default.cpp verifies the Teensy is connected for NanoSAM II tests. 
 * Usage:
 *  run this script through Teensyduino to power the NS2 payload
 *
 */

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries

// NS2 headers

/* - - - - - - Functions - - - - - - */

/* - - - - - - main - - - - - - *
 * Usage:
 *  Main function entered on runtime
 * 
 * Inputs:
 *  none
 *  
 * Outputs:
 *  None
 */
 
 // I think I'm writing this as Arduino code not c++? 
int rHeatAct = 0;
int HI = 3.3;

void loop()
{
    while(true) // run main loop forever
    {
		// Activate Resistor Heater
        digitalWrite(rHeatAct, HI);
		
		// Probably want a delay here, I'm not really sure
		delay(1000)
    }

    return 0;
}

