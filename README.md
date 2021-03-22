# NanoSAM2
This is the repository for all NanoSAM software, including teensy software, ground interface software, and testing software

### Directories ###
FSW - Development for flight software in C++ (using `arduino.h`, `SerialFlash.h`, and `SPI.h` arduino libraries)  
GSW - Development for ground software in C++ and/or Python  
HardwareTest - Small scale, self-contained test sketches for testing Teensy 4.0 functionality with the EPS boards
MATLAB - support scripts to do miscellaneous other work such as calculations
UnitTest - driver script and unit testing scripts for various FSW functions  

### Other Software Needed ### 
Arduino IDE: https://www.arduino.cc/en/software  
Teensyduino addon for the Arduino IDE: https://www.pjrc.com/teensy/teensyduino.html  

### Video Walkthrough: Running the Flight Software on a Teensy 4.0 Microcontroller ###
Here is a video walkthrough of how to run the FSW (or HardwareTest scripts) from scratch on your computer:  
https://youtu.be/LSnabdjjthg

Here is an outline of the basic steps, but make sure to check out the video if you are stuck:
1. Install Arduino IDE and Teensyduino addon (linked above)  
2. Clone this repo and open the local copy on your computer   
3. Copy the contents of the FSW into an Arduino sketch  
    1. Open a new sketch in the Arduino IDE  
    2. Open `FSW/main.cpp` in your local copy of the NS2 repo  
    3. Copy the contents of `main.cpp` into your Arduino sketch  
    4. Save the sketch  
    5. Open the sketch location on your computer  
    6. Copy the directory `FSW/src/` into the Arduino sketch folder  
4. Plug in your microcontroller via USB
    1. Ensure that `Teensy 4.0` is the microcontroller selected in the Arduino IDE
    2. Ensure that the proper USB port is selected under the `Tools/serial` menu item in the Arduino IDE  
5. Open the serial monitor in the Arduino IDE
6. Upload your sketch using the arrow in the top left of the Arduino IDE
7. Verify that information is being output in the serial monitor
8. If you make changes to your local repository, repeat step 3 and re-upload   

### Compilation ###
The FSW file structure is flexible as long as `main.cpp` is in the sketch directory and any other files are included under the directory `src`.  
For NanoSAM II, there are utility and header files, organized into the `util` and `header` dirs respectively. Include these directories under `src` in your arduino sketches.  

