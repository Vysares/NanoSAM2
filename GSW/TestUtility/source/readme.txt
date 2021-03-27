To compile to a single exe with pyinstaller run the following line from the "GSW/TestUtility" directory:
 
pyinstaller.exe --onefile --icon=source/Assets/NS2_BW.ico --windowed --name TestNS2 source/testNS2.py

You can then safely delete the "build" folder and spec file if you want. The exe will be in the "dist" folder.
note that the exe will crash on startup unless the Assets and SavedFiles folders are in the same directory.
