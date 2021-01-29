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
 *  none
 */

/* - - - - - - Includes - - - - - - */
// All libraries are put in memUtil.hpp
// NS2 headers
#include "../headers/memUtil.hpp"

/* Module Variable Definitions */
//TODO: should sampling rate and window length be configurable on flight?
const int SAMPLING_RATE = 50;   // Hz, desired irradiance sampling rate
const int WINDOW_LENGTH = 240;  // seconds, length of science data buffer 
const int MAXFILES = 10;        // maximum number of files in flash storage

// TODO: Update this with size of actual timestamp once it is known
const int TIMESTAMP_SIZE = 1;   // array indices needed to store timestamp


// set number of measurements to store in science data buffer
const int BUFFERSIZE = SAMPLING_RATE * WINDOW_LENGTH; // indices
const int FILESIZE = BUFFERSIZE + TIMESTAMP_SIZE;

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
    Serial.println("Data Processing Module: Test Print");
    float voltage;

    // TODO: STUBBED IN VOLTAGE PLACEHOLDER WHILE I LEARN SPI 
    //       BUT RETURNING THE ACTUAL VOLTAGE WILL BE PRETTY KEY
    voltage = 0; //

    // access ADC Pin (SPI)

    // Convert ADC output to voltage by scaling over voltage range?

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
    Serial.println("Science Memory Handling Module: Test Print");
    
    // TODO: Convert pseudocode for timing to actually call these functions
    //       at the times we need them
    if (time == timeToGatherData){
        float currentPhotodiodeVoltage = dataProcessing();
        
        Serial.print("Current Photodiode Voltage: ");
        Serial.print(currentPhotodiodeVoltage);
        Serial.print("\n");

        updateBuffer(dataBuffer[], currentPhotodiodeVoltage, bufIdx)
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
void updateBuffer(float dataBuffer[], float sample, int &index){
    
    // check that index is valid
    if (0 <= index && index < BUFFERSIZE){
        dataBuffer[index] = sample;
        index++;
    
    } else {
        Serial.print("WARNING: invalid dataBuffer index ")
        Serial.print("(Science Memory Handling Module)\n")
    }

    if (index = BUFFERSIZE){
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
bool saveBuffer(float dataBuffer[], int &index){
    
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
    std::string filestr, extension, filename;
    int fileIdx;
    bool fileFlag = true;

    filestr = "scienceFile";
    extension = ".csv";
    
    while (fileFlag){
        if (fileIdx < MAXFILES){ // prevent infinite loop
            filename = filestr + std::to_string(fileIdx) + extension;
            fileFlag = SerialFlash.exists(filename);
            fileIdx++;
        }
    }

    // create new file (non-erasable, delete file after downlink)
    bool status = true; // track file creation/writing status
    status = SerialFlash.create(filename, FILESIZE);

    // write buffer to this new file
    SerialFlashFile file;
    file = SerialFlash.open(filename);
    file.write(dataBuffer, BUFFERSIZE); //write dataBuffer
    
    // TODO: may need to seek position to end of file before writing again
    //file.seek(BUFFERSIZE);  

    //TODO: decide on timestamp format and append it to file
    //file.write(timestamp);

    index = 0; // reset index to start of array since we have saved the buffer

    return status; // return whether or not file was successfully created
}