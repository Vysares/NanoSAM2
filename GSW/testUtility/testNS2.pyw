import serial
from serial.tools import list_ports
import codecs
import threading
from tkinter import *
import tkinter.scrolledtext as tkScrolledText
import tkinter.messagebox
import os

# ==== setup application window ====
root = Tk()
root.title('NanoSAM II Ground Test')
root.iconbitmap('NS2_BW.ico')

# configure grid
root.grid_columnconfigure(0,weight=0)
root.grid_columnconfigure(1,weight=1)
root.grid_rowconfigure(0,weight=1)
root.grid_rowconfigure(1,weight=1)
root.grid_rowconfigure(2,weight=1)

# ==== create widgets ====
# command frame
commandFrame = LabelFrame(root, text='Send Commands', padx=5, pady=5)
commandField = Entry(commandFrame, width=30, borderwidth=3, font=('Consolas',10))

# log frame
logFrame = LabelFrame(root, text='Log', padx=5, pady=5)
log = tkScrolledText.ScrolledText(logFrame, state='disabled', width=50, height=20, font=('Consolas',10),  wrap=WORD)

# monitor frame
monitorFrame = LabelFrame(root, text='NS2 Output', padx=5, pady=5)
monitor = tkScrolledText.ScrolledText(monitorFrame, state='disabled', width=80, height=30, font=('Consolas',10))
autoScroll = BooleanVar(value=True)
check_autoScroll = Checkbutton(monitorFrame, text='Auto Scroll',variable=autoScroll, onvalue=True, offvalue=False)
check_autoScroll.select()

# file frame
fileFrame = LabelFrame(root, text='Save to File', padx=5, pady=5)
fileNameField = Entry(fileFrame, width=30, borderwidth=3)
fileNameField.insert(0, 'filename')

# serial setup frame
serialFrame = LabelFrame(root, text='Setup Serial', padx=5, pady=5)
portField = Entry(serialFrame, width=30, borderwidth=3)
portField.insert(0, 'COM3')


# ==== define functions and buttons ====
# update log #
def updateLog(text): # writes text to the log
    log.configure(state ='normal')
    log.insert(END, text + '\r\n')
    log.configure(state ='disabled')
    log.see(END)

# send command #
def sendCommand(event=None): # sends a command over serial
    if not portOpen:
        updateLog('No connection!')
        return
    command = commandField.get()
    if command:
        teensy.write(bytes(command, 'utf-8'))
        updateLog('Command Sent: ' + command)
        commandField.delete(0, END)

# read serial #
def readSerial(): # reads incoming communication from NS2 via the open serial port
    data = str(teensy.readline())[2:-1]
    if data:
        monitor.configure(state ='normal')
        monitor.insert(END, codecs.decode(data, "unicode_escape")) # insert data in monitor
        monitor.configure(state ='disabled')
        if autoScroll.get():
            monitor.see(END) 

# scan for serial ports
def scanPorts(): # finds and lists all available com ports
    availablePorts = list_ports.comports()
    updateLog('==== Found Serial Ports: ====')
    for port in availablePorts:
        updateLog('- ' + port.description)
                
# open serial port #
def openSerialPort(): # opens a new serial connection if port exists
    availablePorts = list_ports.comports()
    for port in availablePorts: # if port exists, connect to it
        if portField.get() in port.description:
            teensy.close()
            teensy.port = portField.get()
            teensy.open()
            global portOpen
            portOpen = True
            updateLog('Connected to port \"' + portField.get() + '\".')
            return    
    updateLog('Port \"' + portField.get() + '\" not found.')
    
# save to file #
def saveFile(): # saves monitor contents to a file
    fileName = fileNameField.get() + '.txt'
    filePath = 'saved files\\' + fileNameField.get()
    if (os.path.exists(filePath)): # display warning if file already exists
        warning = '\"' + fileName + '\" already exists. Do you wish to overwrite it?'
        overwrite = tkinter.messagebox.askyesno(title='Warning', message= warning)
        if (not overwrite):
            return
    # write to file
    file = open(filePath,'w', newline = '')
    file.write(monitor.get('1.0','end-1c'))
    updateLog( 'Output saved to ' + fileName)

# clear output #
def clearOutput(): # clears the output monitor
    confirm = tkinter.messagebox.askyesno(title='Warning', message= 'Are you sure you want to clear the output monitor???')
    if confirm:
        monitor.configure(state ='normal')
        monitor.delete('1.0', 'end')
        monitor.configure(state='disabled')
        updateLog('Output monitor cleared.')

# ==== buttons ====
button_sendCommand = Button(commandFrame, text='Send', command=sendCommand)
root.bind('<Return>', sendCommand) # bind the enter key to send command
button_scanPorts = Button(serialFrame, text='Scan For Serial Ports', command=scanPorts)
button_updatePort = Button(serialFrame, text='Open Serial Port', bg='#c9ffba', command=openSerialPort)
button_saveFile = Button(fileFrame, text='Save to File', command=saveFile)
button_clearOutput = Button(monitorFrame, text='Clear', command=clearOutput)

# ==== draw everything ====
# draw command frame
commandFrame.grid(row=0, column=0, padx=5, pady=5, sticky=W+E)
commandField.grid(row=0, column=0, padx=5, pady=3, sticky=W+E)
button_sendCommand.grid(row=0, column=1, sticky=W+E)

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
portField.grid(row=0, column=0, sticky=W+E)
button_updatePort.grid(row=0, column=1, sticky=W+E)
button_scanPorts.grid(row=1, column=0, sticky=W+E, columnspan=2)

# draw file frame
fileFrame.grid(row=3, column=0, padx=5, pady=5, sticky=W+E)
fileNameField.grid(row=0, column=0, sticky=W+E)
button_saveFile.grid(row=0, column=1, sticky=W+E)

# ==== start application ====
# configure serial connection
portOpen = False
teensy = serial.Serial()
teensy.baudrate = 19200
teensy.timeout = 0.05

scanPorts() # scan for serial ports
updateLog('\nTeensy usually appears as \"USB Serial Device\"')
updateLog('Enter the Teensy\'s COM port and click \"Open Serial Port\" to connect.')
commandField.focus() # place cursor in command field

while True:
    root.update()
    if portOpen:
        readSerial()
       
