import serial
from serial.tools import list_ports
import codecs
from tkinter import *
import tkinter.scrolledtext as tkScrolledText
import tkinter.messagebox
import os
import re
import ctypes

# to compile to a single exe with pyinstaller run the following line from the "GSW/TestUtility" directory:
# pyinstaller.exe --onefile --icon=source/Assets/NS2_BW.ico --windowed --name TestNS2 source/testNS2.py
# you can then safely delete the "build" folder and spec file if you want. The exe will be in the "dist" folder.

# ==== setup application window ====
ctypes.windll.shcore.SetProcessDpiAwareness(1)
root = Tk()
root.title('NanoSAM II Testing Utility')
root.iconbitmap('Assets\\NS2_BW.ico')

# configure grid
root.grid_columnconfigure(0,weight=0)
root.grid_columnconfigure(1,weight=1)
root.grid_rowconfigure(1,weight=1)


# ==== create widgets ====
# command frame
commandFrame = LabelFrame(root, text='Send Commands', padx=5, pady=5)
commandField = Entry(commandFrame, width=20, borderwidth=3, font=('Consolas',10))
commandLabel = Label(commandFrame, text='Command Code : ')

# log frame
logFrame = LabelFrame(root, text='Log', padx=5, pady=5)
log = tkScrolledText.ScrolledText(logFrame, state='disabled', width=50, height=30, font=('Consolas',10), wrap=WORD)

# monitor frame
monitorFrame = LabelFrame(root, text='NS2 Output', padx=5, pady=5)
monitor = tkScrolledText.ScrolledText(monitorFrame, state='disabled', width=95, height=10, font=('Consolas',10), wrap=WORD)
autoScroll = BooleanVar(value=True)
check_autoScroll = Checkbutton(monitorFrame, text='Auto Scroll', variable=autoScroll, onvalue=True, offvalue=False)

# file frame
fileFrame = LabelFrame(root, text='Save to File', padx=5, pady=5)
fileNameField = Entry(fileFrame, width=30, borderwidth=3)
fileNameLabel = Label(fileFrame, text='File Name : ')
fileNameField.insert(0, 'super-cool-file')

# serial setup frame
serialFrame = LabelFrame(root, text='Setup Serial', padx=5, pady=5)
portField = Entry(serialFrame, width=20, borderwidth=3)
portField.insert(0, 'COM3')
portLabel = Label(serialFrame, text='Port : ')


# ==== define functions and button commands ====
# update log #
def updateLog(text): # writes text to the log
    log.configure(state ='normal')
    log.insert(END, '> ' + text + '\r\n')
    log.configure(state ='disabled')
    log.see(END)

# update log without > indicator #
def updateLogPlain(text): # writes text to the log without >
    log.configure(state ='normal')
    log.insert(END, text + '\r\n')
    log.configure(state ='disabled')
    log.see(END)

# send command #
def sendCommand(command): # sends command to Teensy
    teensy.write(bytes(command, 'utf-8'))
    updateLog('Command Sent: ' + command)

# read and send command #
def readAndSendCommand(event=None): # sends a command over serial from user input
    if not teensy.isOpen():
        updateLog('No connection!')
        return
    command = commandField.get()
    if command:
        sendCommand(command)
        commandField.delete(0, END)

# read serial #
def readSerial(): # reads incoming communication from NS2 via the open serial port
    try:
        data = str(teensy.readline())[2:-1]
    except: # close the port if exception thrown
        teensy.close()
        updateLog('!!! SERIAL CONNECTION INTERRUPTED !!!')
        return
    
    if data:
        parsedData = codecs.decode(data, "unicode_escape")
        monitor.configure(state ='normal')
        monitor.insert(END, parsedData) # insert data in monitor
        monitor.configure(state ='disabled')
        # write to backup file
        backupFile.write(parsedData)
        backupFile.flush()

        if autoScroll.get():
            monitor.see(END)
            
# scan for serial ports #
def scanPorts(): # finds and lists all available com ports
    availablePorts = list_ports.comports()
    foundPorts = False
    updateLog('==== Found Serial Ports: ====')
    for port in availablePorts:
        updateLogPlain('- ' + port.description)
        foundPorts = True
    if not foundPorts:
        updateLogPlain('No ports found. Is the Teensy connected?')
                
# open serial port #
def openSerialPort(event=None): # opens a new serial connection if port exists
    if not re.match(r"COM[0-9]+", portField.get()):
        updateLog('Port field must take the form \"COM<0-99>\"')
        return
    
    availablePorts = list_ports.comports()
    for port in availablePorts: # if port exists, connect to it
        if portField.get() in port.description:
            teensy.close()
            teensy.port = portField.get()
            teensy.open()
            updateLog('Connected to port \"' + portField.get() + '\"')
            updateLog('Make sure to close the port before reprogramming or disconnecting the Teensy.')
            sendCommand('1');
            return
    updateLog('Port \"' + portField.get() + '\" not found.')

# close serial port #
def closeSerialPort():
    if tkinter.messagebox.askyesno(title='Close Port?', message='Are you sure you want to close the serial connection?'):
        teensy.close()
        updateLog('Port closed. Serial connection terminated.')
    
# save to file #
def saveFile(event=None): # saves monitor contents to a file
    if not fileNameField.get():
        updateLog('Enter a valid file name!')
        return
    fileName = fileNameField.get() + '.txt'
    filePath = 'SavedFiles\\' + fileName
    if (os.path.exists(filePath)): # display warning if file already exists
        warning = '\"' + fileName + '\" already exists in "SavedFiles" folder. Do you wish to overwrite it?'
        overwrite = tkinter.messagebox.askyesno(title='Overwrite?', message= warning)
        if (not overwrite):
            return
    # write to file
    file = open(filePath, 'w', newline='')
    file.write(monitor.get('1.0','end-1c'))
    updateLog( 'Output saved to: \"' + filePath + '\"')

# clear output #
def clearOutput(): # clears the output monitor
    warning = 'Are you sure you want to clear the output monitor? Unsaved data will be lost forever.'
    confirm = tkinter.messagebox.askyesno(title='Clear Output?', message=warning)
    if confirm:
        monitor.configure(state ='normal')
        monitor.delete('1.0', 'end')
        monitor.configure(state='disabled')
        updateLog('Output monitor cleared.')

# on window close
def onClose():
    if tkinter.messagebox.askyesno(title='Quit?', message='Do you want to quit?'):
        root.destroy()
        backupFile.close()
        global running
        running = False
root.protocol("WM_DELETE_WINDOW", onClose)


# ==== buttons ====
button_sendCommand = Button(commandFrame, text='Send', width=10, command=readAndSendCommand)
button_scanPorts = Button(serialFrame, text='Scan For Serial Ports', command=scanPorts)
button_updatePort = Button(serialFrame, text='Open Serial Port', bg='#d3ffcf', command=openSerialPort)
button_closePort = Button(serialFrame, text='Close Serial Port', command=closeSerialPort)
button_saveFile = Button(fileFrame, text='Save to File', width=15, command=saveFile)
button_clearOutput = Button(monitorFrame, text='Clear', width=10, command=clearOutput)

# bind the enter key in each entry frame
commandField.bind('<Return>', readAndSendCommand) 
portField.bind('<Return>', openSerialPort) 
fileNameField.bind('<Return>', saveFile)


# ==== draw everything ====
# draw command frame
commandFrame.grid(row=0, column=0, padx=5, pady=5, sticky=W+E)
commandLabel.grid(row=0, column=0, sticky=W)
commandField.grid(row=0, column=1, padx=5, pady=3, sticky=W+E)
button_sendCommand.grid(row=0, column=2, sticky=W+E)

# draw log frame
logFrame.grid(row=1, column=0, padx=5, pady=5, sticky=N+S+E+W)
log.pack(expand=True, fill=BOTH)

# draw monitor frame
monitorFrame.grid(row=0, column=1, padx=5, pady=5, sticky=N+W+E+S, rowspan=4)
monitor.pack(expand=True, fill=BOTH)
check_autoScroll.pack(side='left')
button_clearOutput.pack(side='right')

# draw serial frame
serialFrame.grid(row=2, column=0, padx=5, pady=5, sticky=W+E)
portLabel.grid(row=0, column=0, sticky=W)
portField.grid(row=0, column=1, sticky=E)
button_updatePort.grid(row=0, column=2, padx=3, pady=3, sticky=W+E)
button_closePort.grid(row=0, column=3, padx=3, pady=3, sticky=W+E)
button_scanPorts.grid(row=1, column=0, padx=3, pady=3, sticky=W+E, columnspan=4)

# draw file frame
fileFrame.grid(row=3, column=0, padx=5, pady=5, sticky=W+E)
fileNameLabel.grid(row=0, column=0, sticky=W+N)
fileNameField.grid(row=0, column=1, sticky=W+E)
button_saveFile.grid(row=0, column=2, sticky=W+E)


# ==== start application ====
running = True;
# configure serial connection
teensy = serial.Serial()
teensy.baudrate = 19200
teensy.timeout = 0.05

# configure file
backupFile = open('SavedFiles\\backup.txt', 'w', newline='')

# brief the user
scanPorts() # scan for serial ports
updateLogPlain('')
updateLog('Teensy usually appears as \"USB Serial Device\"')
updateLog('Enter the Teensy\'s COM port and click \"Open Serial Port\" to connect.')
updateLog('The Arduino serial monitor must be closed before attempting to connect!')
updateLogPlain('')
commandField.focus() # place cursor in command field

# Main loop
while running:
    if teensy.isOpen():
        readSerial()
    root.update()
