/* default.cpp verifies the Teensy is connected for NanoSAM II tests.
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
const int PIN_AREG_CURR = 17;   // analog regulator current pin
const int PIN_DREG_CURR = 18;   // digital regulator current pin
const int PIN_DREG_PG = 20;     // digital regulator 'power good' pin
const int PIN_WD_RESET = 2;     // watchdog reset pin

// Acceptable voltage range
const float VMIN = 3.25;        // minimum accpetable voltage
const float VMAX = 3.35;        // maximum acceptable voltage

// timing
const int SAMPLE_INTERVAL = 3000;       // time between samples in ms
const int WD_RESET_INTERVAL = 100;     // watchdog feeding interval, ms
const int WD_PULSE_DUR = 10;            // watchdog reset signal duration, MICROSECONDS!!!

int wdLastFeedMillis;
int lastSampleMillis;

// conversion factors
const int ADC_BINS = 1023;                      // number of ADC bins
const float HI_VOLTAGE = 3.3;                   // high board voltage, 3.3V for teensy
const float VOLTAGE_RES = HI_VOLTAGE/ADC_BINS;   // voltage per bin as read by analogRead()


/* - - - - - - Functions - - - - - - */

/* - - - - - printStatus - - - - - */
void printStatus()
{
  float aCurr = VOLTAGE_RES*(float)analogRead(PIN_AREG_CURR);   // analog regulator current, Amps
  float dCurr = VOLTAGE_RES*(float)analogRead(PIN_DREG_CURR);   // digital regulator current, Amps
  float dRegPG = VOLTAGE_RES*(float)analogRead(PIN_DREG_PG);    // digital regulator voltage 

  // Print statements for confirmation
  Serial.print("\nAnalog Current Value [A]: ");
  Serial.print(aCurr);
  Serial.print("\nDigital Current Value [A]: ");
  Serial.print(dCurr);
  Serial.print("\nDigital Power Good [V]: ");
  Serial.print(dRegPG);
  

  // if the power good signal returns around 3.3V, power is good
  if (dRegPG > VMIN && dRegPG < VMAX)
  {
    Serial.println("\nGood to go.");
  }
  else
  {
    Serial.print("\nVoltage out of nominal range: ");
    Serial.print(VMIN);
    Serial.print(" - ");
    Serial.println(VMAX);
  }
}

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
  pinMode(PIN_AREG_CURR, INPUT);
  pinMode(PIN_DREG_CURR, INPUT);
  pinMode(PIN_DREG_PG, INPUT);
  pinMode(PIN_WD_RESET, OUTPUT);
  Serial.begin(9600);

  //feed the dog
  wdLastFeedMillis = millis();
  feedDog();

  lastSampleMillis = millis();
  printStatus();
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

  // sampling data
  if (millis() >= lastSampleMillis + SAMPLE_INTERVAL)
  {
    lastSampleMillis = millis();
    printStatus();
  }
}