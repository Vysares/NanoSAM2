/* lightCollect.cpp tests the initial photodiode capability
 * Usage:
 *  run this script in sequence with "Electronics System Test" document
 *  in order to validate the photodiode works
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
 
int sciData = 14;

void loop()
{
    while(true) // run main loop forever
    {
		// Read the internal ADC
        sci = analogRead(sciData);
		
		// TODO: Read the actual ADC via SPI
		
		// Print statements for confirmation
		Serial.print('Recorded Science Voltage [V]: ');
		Serial.print(sci);
		Serial.print('\n');
		
        break;
    }

    return 0;
}

