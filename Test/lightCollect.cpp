/* lightCollect.cpp verifies the Teensy is collecting photodiode data.
 * Usage:
 *  run this script through Teensyduino to power the NS2 payload
 *
 */

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries
#include <Arduino.h>
#include <SPI.h>

// NS2 headers

/* - - - - - - Initialization - - - - - - */
// pin numbers
const int PIN_ADC_CS = 10;      // ADC chip select pin
const int PIN_PHOTO = 14;       // direct photodiode data pin for use with Teensy ADC
const int PIN_WD_RESET = 2;     // watchdog reset pin

// timing
const int SAMPLE_INTERVAL = 500;        // time between samples in ms
const int WD_RESET_INTERVAL = 1000;     // watchdog feeding interval, ms
const int WD_PULSE_DUR = 10;            // watchdog reset signal duration, MICROSECONDS!!!

int wdLastFeedMillis;
int lastSampleMillis;

//SPI settings
SPISettings adcSettings(2000000,MSBFIRST, SPI_MODE3); // max serial clock speed, data order, data mode

// conversion factors
const int ADC_BINS = 1024;
const float HI_VOLTAGE = 3.3;


/* - - - - - - Functions - - - - - - */

/* - - - - - readDataBackup - - - - - */
void readDataBackup() // read data direct from pin using Teensy ADC
{
    double sciData = HI_VOLTAGE*(float)analogRead(PIN_PHOTO)/ADC_BINS;
    Serial.print(sciData);
}

/* - - - - - readDataSPI - - - - - */
void readDataSPI() // read data from board ADC via SPI. No idea if this works.
{
    SPI.beginTransaction(adcSettings);
    digitalWrite (PIN_ADC_CS, LOW);         // pull CS low to select ADC
    double sciData = SPI.transfer(0);       // read only, sent data does not matter. May need to be a uint16 type??? 
    digitalWrite (PIN_ADC_CS, HIGH);        // deselect ADC
    SPI.endTransaction();
    Serial.println(sciData);
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
    pinMode(PIN_PHOTO, INPUT);
    pinMode(PIN_ADC_CS, OUTPUT);
    pinMode(PIN_WD_RESET, OUTPUT);

    // setup comms
    Serial.begin(9600);
    SPI.begin();

    //feed the dog
    wdLastFeedMillis = millis();
    feedDog();

    Serial.println("Photodiode Voltage [V]:");
    Serial.println("[Teensy ADC] , [SPI]\n");
    lastSampleMillis = millis();
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
        readDataBackup();   // read from pin
        Serial.print(" , ");
        readDataSPI();      // read from SPI
    }
}
