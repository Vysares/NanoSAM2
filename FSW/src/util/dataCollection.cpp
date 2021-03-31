/* dataCollection.cpp handles NS2 payload memory allocation and assignment 
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
 *  edac.cpp & edac.hpp
 */

/* - - - - - - Includes - - - - - - */
// All libraries are put in dataCollection.hpp
// NS2 headers
#include "../headers/dataCollection.hpp"
#include "../headers/timing.hpp"
#include "../headers/encodedSciData.hpp"

/* Module Variable Definitions */

// declare static variables so that they do not go away when we leave this module
static int bufIdx = 0;               // index of next dataBuffer element to overwrite
static uint16_t dataBuffer[BUFFERSIZE]; // create array to hold data buffer elements

// file reading/writing
static char filename[] = "scienceFile0.csv";   // null-terminated char array 
static const int FILE_IDX_OFFSET = 11;           // index of file number in char array

// SPI 
// may want to update this to dynamically change the flash module, keeping it simple for now
static const int CURRENT_FLASH_CHIP = PIN_FLASH1_CS;

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
 *  data from the ADC (bin number)
 */
uint16_t dataProcessing() {

    // begin an SPI connection
    // TODO: we should probably begin the SPI connection in main to avoid duplicates
    SPI.begin();  
    pinMode(PIN_ADC_CS, OUTPUT); // set ADC chip select pin to output

    // access ADC Pin (SPI)
    uint16_t photodiode16; // 16 bit variable to hold bin number from ADC
    SPI.beginTransaction(SPISettings(ADC_MAX_SPEED, MSBFIRST, SPI_MODE3)); //SPISettings(maxSpeed,dataOrder,dataMode)
    digitalWrite(PIN_ADC_CS, LOW);   // set Slave Select pin to low to select chip
    photodiode16 = SPI.transfer16(0x0000);// transfer data, send 0 to slave, recieve data from ADC
    digitalWrite(PIN_ADC_CS, HIGH);  // set Slave Select pin to high to de-select chip
    SPI.endTransaction();

    /* NOTE TO FUTURE TEAMS:
     *      Append ADCS attitude (direction payload is pointing) to the voltage here
     *      pointing data it essential for profiling the aerosol contents
     *      of the atmosphere, if we do not have pointing data associated with
     *      the voltage measurement we do not actually achieve any science
     * 
     *      However, NS2 did not have enough info about what form the pointing data
     *      for the cubesat would take, so we figured selecting a format would only
     *      lead to more work for future teams trying to retrofit our format
     */

    // print the voltage value (for testing)
    if (printPhotoEvent.checkInvoked() || STREAM_PHOTO) {
        float spiVoltage = photodiode16 * ADC_VOLTAGE_RES;
        float directVoltage = analogRead(PIN_PHOTO) * TEENSY_VOLTAGE_RES;
        Serial.print("PHOTO, ");
        Serial.print(millis());
        Serial.print(", ");
        Serial.print(spiVoltage);
        Serial.print(", ");
        Serial.println(directVoltage);
    }
    // return the bin number
    return photodiode16;
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
void scienceMemoryHandling() {    
    if (dataProcessEvent.checkInvoked()) { // checks event status from timing module
        uint16_t photodiodeVoltage = dataProcessing();
        updateBuffer(photodiodeVoltage, bufIdx);

        // determine which mode the payload is in to act on this data properly
        updatePayloadMode(dataBuffer, bufIdx); // from timing module
    }

    if (saveBufferEvent.checkInvoked()) {
        saveBuffer();
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
void updateBuffer(uint16_t sample, int &index) {
    
    // check that index is valid
    if (0 <= index && index < BUFFERSIZE) {
        dataBuffer[index] = sample;
        index++;
    
    } else {
        Serial.print("WARNING: invalid dataBuffer index ");
        Serial.println("(Science Memory Handling Module - updateBuffer() func)");
    }

    if (index == BUFFERSIZE) {
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
 *  None
 *  
 * Outputs:
 *  file creation status
 */
bool saveBuffer() {
    
    // create array to hold data in time ascending order
    uint16_t timeSortBuffer[BUFFERSIZE];
    int j = 0; // iterator for sorted array index

    // reorder array so that it is ascending in time
    for (int i = bufIdx; i < BUFFERSIZE; i++) {
        timeSortBuffer[j] = dataBuffer[i];
        j++;
    } 
    // loop back to top of array and store remaining values
    for (int i = 0; i < bufIdx; i++) {
        timeSortBuffer[j] = dataBuffer[i];
        j++;
    }
    
    // compute timestamp
    unsigned long timestamp = calcTimestamp(); 
    
    // Run all the data through EDAC
    EncodedSciData encodedFileData = EncodedSciData(timeSortBuffer, timestamp); // this one line cost 50+ hours of my life

    /* send sorted array to file on flash memory along with timestamp 
     * see SerialFlash docs for info on these functions
     * https://github.com/PaulStoffregen/SerialFlash/blob/master/README.md 
     */

    // check if file exists
    int fileIdx = 0; // iterator for loop checking file existence
    bool fileFlag = true; // true until a nonexistent file found

    while (fileFlag) {
        if (fileIdx < MAXFILES){ // prevent infinite loop
            filename[FILE_IDX_OFFSET] += 1; // iterate up from zero
            fileFlag = SerialFlash.exists(filename);
            fileIdx++;
        } else if (fileIdx >= MAXFILES) {
            Serial.print("WARNING: fileIdx reached MAXFILES.");
            Serial.println("(Science Memory Handling Module - saveBuffer() func)");
        }
    }

	// establish SPI connection to flash chip
	bool status = true; // track file creation/writing status
	
	if (SerialFlash.begin(CURRENT_FLASH_CHIP)) { // SPI to flash module successful

		// create new file (non-erasable, delete file after downlink)
		status = SerialFlash.create(filename, encodedFileData.MEMSIZE);

		if (status) { Serial.print("Successfully created file: "); }
        else { Serial.print("Failed to create file: "); }
        Serial.println(filename);

		// write buffer to this new file
		SerialFlashFile file;
		file = SerialFlash.open(filename);
		status = file.write(encodedFileData.getData(), encodedFileData.MEMSIZE); // write encoded science data to file

        if (status) { Serial.print("Write successful: "); }
        else { Serial.print("Write failed: "); }
        Serial.println(filename);

        bufIdx = 0; // reset index to start of array since we have saved the buffer

	} else { // SerialFlash connection failed

		Serial.println("Failed to establish SerialFlash connection to current flash module (saveBuffer() func)");
		status = false;
	}

    return status; // return whether or not file was successfully created
}

/* - - - - - - calcTimestamp - - - - - - *
 * Belongs to Science Memory Handling Module
 *  
 * Usage:
 *  computes the relative time between teensy startup and when this file is saved
 *  returns relative time be appended to the end of the file   
 *  NOTE: in the file, only the timestamp of the final data point will be 
 *        available. Use GSW and the sampling rate of FSW to backsolve for the 
 *        timestamp of all preceding data points
 * 
 * FOR FUTURE TEAMS:
 * this year's time is relative since we do not have a bus clock signal
 *   however, it may be desirable to downlink the times in UTC or another standardized
 *   time so that you do not have to keep track of the time that the teensy powered on
 * By piping in a clock signal from the bus, you could potentially use the 
 *  teensy time library to append timestamp
 *  https://www.pjrc.com/teensy/td_libs_Time.html
 *  might be useful to convert timestamp to ISO 8601 timekeeping standard for storing to file
 *       ([YYYY]-[MM]-[DD]T[hh]:[mm]:[ss].[sss] where [sss] is subseconds)
 * 
 * Inputs:
 *  none
 *  
 * Outputs:
 *  relative timestamp
 */
unsigned long calcTimestamp() {
    unsigned long currentTimeRelative = millis(); // milliseconds    

    // FUTURE TEAMS: offset this relative time by the known time from the bus so that 
    // a UTC (or some other standard) time is downlinked instead of a relative time

    // for now we just return relative time
    unsigned long timestamp = currentTimeRelative; 

    return timestamp;
}

/* - - - - - - downlink - - - - - - *
 *  
 * Usage:
 *  downlinks all files on the flash module
 * 
 * Inputs:
 *  none
 *  
 * Outputs:
 *  none
 */
void downlink() {
    static char downlinkFileName[] = "scienceFile0.csv";
    static int downlinkFileCount = 0;

    // reset static variables at start of new event
    if (downlinkEvent.first()) {
        downlinkFileCount = 0;
    }

    // TODO: make flash chip we are using configurable with a command? Auto balance the load somehow?
    SerialFlash.begin(CURRENT_FLASH_CHIP); // begin spi connection with flash module 1

    /* downlink a single file */
    downlinkFileName[FILE_IDX_OFFSET] = '0' + (downlinkEvent.iter() - 1); // update file name 
    if (SerialFlash.exists(filename)) { // check if file exists
        Serial.print("Downlinking ");
        Serial.print(downlinkFileName);
        Serial.println(":");


        // read data from file into downlink buffer
        SerialFlashFile file;
        file = SerialFlash.open(downlinkFileName);
        if (file) {
            char downlinkBuffer[EncodedSciData::MEMSIZE];
            file.read(downlinkBuffer, EncodedSciData::MEMSIZE);
            Serial.println(downlinkBuffer);
            Serial.println(); // skip a line between files
            downlinkFileCount++;
        }
        // remove file
        // this allows a file of the same name to overwrite this data in the future
        SerialFlash.remove(downlinkFileName);
    }

    // print report at end of event
    if (downlinkEvent.over()) { 
        Serial.print("Downlink complete - ");
        Serial.print(downlinkFileCount);
        Serial.println(" file(s).");
    }
}

/* - - - - - - scrubFlash - - - - - - *
 *  
 * Usage:
 *  Scrubs all files in flash memory.
 *  This function should only be called when scrubEvent is invoked
 * 
 * Inputs:
 *  None
 * Outputs:
 *  None
 */
void scrubFlash() {
    // local static variables are only initialized once
    static char scrubFilename[] = "scienceFile0.csv"; 
    static ScrubReport totalScrubInfo;

    EncodedSciData correctedFileData;
    ScrubReport scrubInfo;
    
    // reset static variables at start of new event
    if (scrubEvent.first()) {
        totalScrubInfo.numErrors = 0;
        totalScrubInfo.corrected = 0;
        totalScrubInfo.uncorrected = 0;
    }
    
    /* scrub a single file */
    scrubFilename[FILE_IDX_OFFSET] = '0' + (scrubEvent.iter() - 1); // update file name
    if (SerialFlash.exists(scrubFilename)) { // check if file exists
        // read data from file and scrub it
        SerialFlashFile file;
        file = SerialFlash.open(scrubFilename);
        if (file) {
            uint8_t fileContents[EncodedSciData::MEMSIZE];
            file.read(fileContents, EncodedSciData::MEMSIZE);
            correctedFileData.fill(fileContents);
            scrubInfo = correctedFileData.scrub(); // scrub it

            // update total scrub info
            totalScrubInfo.corrected += scrubInfo.corrected;
            totalScrubInfo.numErrors += scrubInfo.numErrors;
            totalScrubInfo.uncorrected += scrubInfo.uncorrected;
        }
        
        // replace corrupted file with corrected file
        if (scrubInfo.numErrors > 0) {
            SerialFlash.remove(scrubFilename); // remove corrupted file
            
            // create new file and write corrected data
            bool status = SerialFlash.create(scrubFilename, EncodedSciData::MEMSIZE);
            file = SerialFlash.open(scrubFilename);
            status = file.write(correctedFileData.getData(), EncodedSciData::MEMSIZE); // write encoded science data to file
        }
    }

    // print report at end of event
    if (scrubEvent.over()) { 
        Serial.print("Scrub complete - found errors in ");
        Serial.print(totalScrubInfo.numErrors);
        Serial.print(" block(s), ");
        Serial.print(totalScrubInfo.corrected);
        Serial.print(" corrected, ");
        Serial.print(totalScrubInfo.uncorrected);
        Serial.println(" cleared.");
    }
}
