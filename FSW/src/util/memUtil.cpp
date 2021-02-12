/* memUtil.cpp handles NS2 payload memory allocation and assignment 
 * Usage:
 *  module functionality
 *  function definitions
 *  put function declarations in memUtil.hpp
 * 
 * Modules encompassed:
 *  Data Processing
 *  Science Memory Handling
 *
 * Additional files needed for compilation:
 *  config.hpp
 *  timing.cpp & timing.hpp
 */

/* - - - - - - Includes - - - - - - */
// All libraries are put in memUtil.hpp
// NS2 headers
#include "../headers/config.hpp"
#include "../headers/memUtil.hpp"
//#include "../headers/timing.hpp"
#include "../headers/timingDeclarations.hpp"

/* Module Variable Definitions */

// declare static variables so that they do not go away when we leave this module
// whole purpose is to make this into a black-box module of sorts
static int bufIdx = 0;               // index of next dataBuffer element to overwrite
static float dataBuffer[BUFFERSIZE]; // create array to hold data buffer elements

/* - - - - - - Module Driver Functions - - - - - - */

/* - - - - - - dataProcessing - - - - - - *
 * Usage:
 *  Reads incoming data from ADC, measures against baseline,
 *  sends to memory.
 * 
 * Inputs:
 *  none
 *  
 * Outputs:
 *  data from the ADC (voltage measurement)
 */
float dataProcessing()
{
    float voltage;

    // begin an SPI connection
    // TODO: we should probably begin the SPI connection in main to avoid duplicates
    SPI.begin();  
    pinMode(PIN_ADC_CS, OUTPUT); // set ADC chip select pin to output

    // access ADC Pin (SPI)
    uint16_t photodiode16; 
    SPI.beginTransaction(SPISettings(SPI_MAX_SPEED, MSBFIRST, SPI_MODE3)); //SPISettings(maxSpeed,dataOrder,dataMode)
    digitalWrite(PIN_ADC_CS, LOW);   // set Slave Select pin to low to select chip
    photodiode16 = SPI.transfer16(0x0000);// transfer data, send 0 to slave, recieve data from ADC
    digitalWrite(PIN_ADC_CS, HIGH);  // set Slave Select pin to high to de-select chip
    SPI.endTransaction();

    /* NOTE TO FUTURE TEAMS:
     *      Append pointing data to the voltage here
     *      pointing data it essential for profiling the aerosol contents
     *      of the atmosphere, if we do not have pointing data associated with
     *      the voltage measurement we do not actually achieve any science
     * 
     *      However, NS2 did not have enough info about what form the pointing data
     *      for the cubesat would take, so we figured selecting a format would only
     *      lead to more work for future teams trying to retrofit our format
     */

    // convert from Bin number to voltage, assuming board voltage does not fluctuate
    voltage = photodiode16 / ADC_BINS * (ADC_MAX_VOLTAGE - ADC_MIN_VOLTAGE);

    return voltage;
}

/* - - - - - - scienceMemoryHandling - - - - - - *
 * Usage:
 *  Takes data from processing and stores it in a buffer,
 *  continuously overwriting old entries.
 *  Copies buffer to long term memory when signaled by timing module.
 * 
 * Inputs:
 *  none
 *  
 * Outputs:
 *  None
 */
void scienceMemoryHandling()
{    
    if (dataProcessEvent.checkInvoked()){ // checks event status from timing module
        float photodiodeVoltage = dataProcessing();
        updateBuffer(photodiodeVoltage, bufIdx);

        // determine which mode the payload is in to act on this data properly
        updatePayloadMode(dataBuffer, bufIdx); // from timing module

        Serial.print("Photodiode Voltage: ");
        Serial.print(photodiodeVoltage);
        Serial.print(" - Payload Mode ");
        Serial.println(scienceMode.get());
    }

    if (saveBufferEvent.checkInvoked()){
        saveBuffer(bufIdx);
    }
}

/* - - - - - - Helper Functions - - - - - - */

/* - - - - - - updateBuffer - - - - - - *
 * Belongs to Science Memory Handling Module
 *  
 * Usage:
 *  adds a new sample to the data buffer at specified index
 *  an existing element at index will be overwritten
 * 
 * Inputs:
 *  dataBuffer - pointer to first element of data buffer
 *  sample - data to be stored (type must match dataBuffer type)
 *  index - index to store new data at (pass-by-reference)
 *  
 * Outputs:
 *  none
 */
void updateBuffer(float sample, int &index){
    
    // check that index is valid
    if (0 <= index && index < BUFFERSIZE){
        dataBuffer[index] = sample;
        index++;
    
    } else {
        Serial.print("WARNING: invalid dataBuffer index ");
        Serial.print("(Science Memory Handling Module - updateBuffer() func)\n");
    }

    if (index == BUFFERSIZE){
        // reset index if we have reached end of buffer
        index = 0; 
    }  
}

/* - - - - - - saveBuffer - - - - - - *
 * Belongs to Science Memory Handling Module
 *  
 * Usage:
 *  saves the buffer array passed in to a file on the flash module in csv format
 *  appends timestamp to the end of the file
 * 
 * Inputs:
 *  dataBuffer - pointer to first element of data buffer
 *  index - index to store new data at (pass-by-reference)
 *  
 * Outputs:
 *  file creation status
 */
bool saveBuffer(int &index){
    
    // create array to hold data in time ascending order
    float timeSortArray[BUFFERSIZE];
    int j = 0; // iterator for sorted array index

    // reorder array so that it is ascending in time
    for (int i = index; i < BUFFERSIZE; i++){
        timeSortArray[j] = dataBuffer[i];
        j++;
    } 

    // loop back to top of array and store remaining values
    for (int i = 0; i < index; i++){
        timeSortArray[j] = dataBuffer[i];
        j++;
    }

    /* send sorted array to file on flash memory along with timestamp 
     * see SerialFlash docs for info on these functions
     * https://github.com/PaulStoffregen/SerialFlash/blob/master/README.md 
     */

    // check if file exists
    int fileIdx = 0; // iterator for loop checking file existence
    bool fileFlag = true;
    char filename[] = "scienceFile0.csv";   // null-terminated char array
    int fileIdxOffset = 11;                 // index of file number

    while (fileFlag){
        if (fileIdx < MAXFILES){ // prevent infinite loop
            filename[fileIdxOffset] += fileIdx; // iterate up from zero
            fileFlag = SerialFlash.exists(filename);
            fileIdx++;
        } else if (fileIdx >= MAXFILES) {
            Serial.print("WARNING: fileIdx reached MAXFILES ");
            Serial.print("(Science Memory Handling Module - saveBuffer() func)\n");
        }
    }

    // create new file (non-erasable, delete file after downlink)
    bool status = true; // track file creation/writing status
    status = SerialFlash.create(filename, FILESIZE);

    if (status){
        Serial.print("Found file ");
        Serial.print(filename);
        Serial.print(" on flash chip");
    }

    // write buffer to this new file
    SerialFlashFile file;
    file = SerialFlash.open(filename);
    status = file.write(dataBuffer, BUFFERSIZE); //write dataBuffer
    
    // TODO: may need to seek position to end of file before writing again
    //file.seek(BUFFERSIZE);  

    //TODO: decide on timestamp format and append it to file
    //file.write(timestamp);

    index = 0; // reset index to start of array since we have saved the buffer

    return status; // return whether or not file was successfully created
}