# NanoSAM2
This is the repository for all NanoSAM software, including teensy software, ground interface software, and EDAC testing software

### Compilation ###
To compile and run main.cpp run the following command:
g++ main.cpp util/memUtil.cpp -o NS2_main && ./NS2_main

Make sure to add in any other helper files as they are added
Keep relative paths accurate if file structure changes