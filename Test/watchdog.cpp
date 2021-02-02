/* wathcdog.cpp verifies the Teensy can feed the watchdog and that the watchdog triggers a restart.
 * Usage:
 *  run this script through Teensyduino to power the NS2 payload
 *  Send "1" over the serial monitor to trigger a restart
 */

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries
#include <Arduino.h>

// NS2 headers

/* - - - - - - Initialization - - - - - - */
// pin numbers
const int PIN_WD_RESET = 2;     // watchdog reset pin

// timing
const int WD_RESET_INTERVAL = 1000;     // watchdog feeding interval, ms
const int WD_PULSE_DUR = 10;            // watchdog reset signal duration, MICROSECONDS!!!
const int SERIAL_TIMEOUT = 50;          // time to wait for serial input, ms

int wdLastFeedMillis;
int serialInput = 0;

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
    pinMode(PIN_WD_RESET, OUTPUT);
    Serial.begin(9600);
    Serial.setTimeout(SERIAL_TIMEOUT);
    //feed the dog
    wdLastFeedMillis = millis();
    feedDog();

}

/* - - - - - Main Loop - - - - - */
void loop()
{
    // read Serial
    if (Serial.available() > 0)
    {
        serialInput = Serial.parseInt();
        // enter "1" to force a restart
        if (serialInput == 1)
        {
            // push next watchdog reset wayyy over the maximum time limit
            wdLastFeedMillis += 1000*WD_RESET_INTERVAL; 
            Serial.println("Watchdog feeding paused. Restart imminent!");
        }
    }

    // watchdog reset
    if (millis() >= wdLastFeedMillis + WD_RESET_INTERVAL)
    {
        wdLastFeedMillis = millis(); // restart the clock
        feedDog();

        // flash LED so you can look with your special eyes.
        digitalWrite(LED_BUILTIN,HIGH);
        delay(50); // this delay is small enough to not mess with the wd
        digitalWrite(LED_BUILTIN,LOW);
    }
}
