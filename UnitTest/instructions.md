## Unit Test Instructions
All software compiled on the teensy has to be nested in a `src` directory, so that makes this file structure take a form that may not be intuitive. 
To run a test on the teensy, first make sure your `FSW/src` directory is up to date with the latest version of the FSW that you want to test. 
Then, copy the contents of `UnitTest/TestScripts` into `FSW/src`
This directory in the arduino sketch will now contain at least three folders, `FSW/src/headers`, `FSW/src/util`, and now `FSW/src/TestScripts`
Once this is complete, you can open `unitTestDriver.c` as your main script in the Arduino IDE and compile and run as usual. 