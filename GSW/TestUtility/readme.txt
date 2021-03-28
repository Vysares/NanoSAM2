The "extecutable" directory contains a ready to run .exe file. Note that it will only run on Windows systems.
The "source" directory contains the python source code.

===== For new builds =====
To compile to a single exe with pyinstaller run the following line from the "GSW/TestUtility" directory:

pyinstaller.exe --onefile --icon=source/Assets/NS2_BW.ico --windowed --name TestNS2 source/testNS2.py

You can then safely delete the "build" folder and spec file if you want. The exe will be in the "dist" folder.
Note that the exe must be in the same directory as Assets/NS2_BW to correctly display the cool icon.