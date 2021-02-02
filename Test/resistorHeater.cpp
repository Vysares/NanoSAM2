/* resistorHeater.cpp verifies the Teensy can control the heating resistor.
 * Usage:
 *  run this script through Teensyduino to power the NS2 payload
 *
 */

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries
#include <Arduino.h>

// NS2 headers

/* - - - - - - Initialization - - - - - - */
// pin numbers
const int PIN_HEAT = 0;         // activate heater pin
const int PIN_WD_RESET = 2;     // watchdog reset pin

// timing
const int WD_RESET_INTERVAL = 100;     // watchdog feeding interval, ms
const int WD_PULSE_DUR = 10;            // watchdog reset signal duration, MICROSECONDS!!!

int wdLastFeedMillis;

/* - - - - - - Functions - - - - - - */

/* - - - - - feedDog - - - - - */
void feedDog()
{
    digitalWrite(PIN_WD_RESET,HIGH);
    delayMicroseconds(WD_PULSE_DUR);
    digitalWrite(PIN_WD_RESET,LOW);
}

/* - - - - - Setup - - - - - */
void setup()
{
    // set pin modes
    pinMode(PIN_HEAT, OUTPUT);
    pinMode(PIN_WD_RESET, OUTPUT);
    Serial.begin(9600);

    //feed the dog
    wdLastFeedMillis = millis();
    feedDog();

    // Activate heater
    digitalWrite(PIN_HEAT, HIGH);
    Serial.println("Heater on.");
}

/* - - - - - Main Loop - - - - - */
void loop()
{
    // watchdog reset
    if (millis() >= wdLastFeedMillis + WD_RESET_INTERVAL)
    {
        wdLastFeedMillis = millis();
        feedDog();
    }
}
