# NanoSAM2
This is the repository for all NanoSAM software, including teensy software, ground interface software, and EDAC testing software

### Directories ###
FSW - Development for flight software in C++ (using `arduino.h`, `SerialFlash.h`, and `SPI.h` arduino libraries)
GSW - Development for ground software in C++ and/or Python
Arduino - FSW build configured to compile in the Arduino IDE

### Compilation ###
The FSW file structure is flexible as long as `main.cpp` is in the sketch directory and any other files are included under the directory `src`.
For NanoSAM II, there are utility and header files, organized into the `util` and `header` dirs respectively. Include these directories under `src` in your arduino sketches.
Then, open `main.cpp` in the Arduino IDE, add the `src` directory and all nested files/folders, and compile using the checkmark or arrow in the top left. 
Arduino IDE: https://www.arduino.cc/en/software

Make sure you have the Teensyduino addon for the Arduino IDE if uploading sketches to the Teensy:
https://www.pjrc.com/teensy/teensyduino.html
