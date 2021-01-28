/* default.cpp verifies the Teensy is connected for NanoSAM II tests. 
 * Usage:
 *  run this script through Teensyduino to power the NS2 payload
 *
 */

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries

// NS2 headers
 
 // I think I'm writing this as Arduino code not c++? 
int aRegCurr = 17;
int dRegCurr = 18;
int dRegPG = 20; 

void loop()
{
    while(true) // run main loop forever
    {
		// Check voltage regulator currents
        aCurr = analogRead(aRegCurr);
		dCurr = analogRead(dRegCurr);
		dPG = analogRead(dRegPG);
		
		
		// Print statements for confirmation
		// TODO: Gotta be a better way to format these statements?
		Serial.print('Analog Current Value [A]: ');
		Serial.print(aCurr);
		Serial.print('\n');
		
		Serial.print('Digital Current Value [A]: ');
		Serial.print(dCurr);
		Serial.print('\n');
		
		Serial.print('Digital Power Good [V]: ');
		Serial.print(dPG);
		Serial.print('\n');
		
		// if the power good signal returns around 3.3V, power is good
		// TODO: I have no clue how to write an if statement in arduino
		if (dPG > 3.25) && (dPG < 3.35)
			Serial.print('Good to go')
		end
		
        break;
    }

    return 0;
}

