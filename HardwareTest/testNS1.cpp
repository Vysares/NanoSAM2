/* testNS1.cpp continuously reads photodiode data and transmits each data point via serial
 * Usage:
 *  This is a quick and dirty script based on the original NS1 software to test the basic functionality of NanoSam I
 *  Run this script through Teensyduino to test the NS1 payload, monitor via the serial monitor/plot
 *  
 */

/* - - - - - - Includes - - - - - - */
// C++ libraries

// Other libraries
#include <SPI.h>

/* - - - - - - Initialization - - - - - - */
const int ADC_CHIP_SELECT = 10;  //chip select pin number for ADC
const int sampleInterval = 100;  // interval between samples in ms

/* - - - - - - Functions - - - - - - */

// getADC reads and returns one 16 bit data point from the ADC via SPI
uint16_t getADC()
{
  uint16_t photodiode16; 
  SPI.beginTransaction(SPISettings(50, MSBFIRST, SPI_MODE3)); //SPISettings(maxSpeed,dataOrder,dataMode)
  digitalWrite(ADC_CHIP_SELECT, LOW);   // set Slave Select pin 10 to low to select chip
  photodiode16 = SPI.transfer16(0x00);  // transfer data
  digitalWrite(ADC_CHIP_SELECT, HIGH);  // set Slave Select pin 10 to high to de-select chip
  SPI.endTransaction();
  return photodiode16;
}

void setup()
{
  SPI.begin();  
  pinMode(ADC_CHIP_SELECT, OUTPUT); // set ADC chip select pin to output
  
  Serial.begin(9600);
  while (!Serial); // wait for serial to be ready
  Serial.println("Serial ready to go. Here (hopefully) comes the data:");
  Serial.flush();
}

void loop()
{
  Serial.println(getADC()); //print the ADC voltage
  delay(sampleInterval);
}
