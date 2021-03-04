import serial
from serial.tools import list_ports
import codecs
import threading
from tkinter import *
import tkinter.scrolledtext as tkScrolledText

        
# ==== setup application window ====
root = Tk()
root.title('NanoSAM II Ground Test')
#root.resizable(False, False)
root.iconbitmap('NS2_emblem.ico')

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
log = tkScrolledText.ScrolledText(logFrame, state='disabled', width=50, height=20, font=('Consolas',10))

# monitor frame
monitorFrame = LabelFrame(root, text='NS2 Output', padx=5, pady=5)
monitor = tkScrolledText.ScrolledText(monitorFrame, state='disabled', width=80, height=30, font=('Consolas',10))

# serial setup frame
serialFrame = LabelFrame(root, text='Setup Serial', padx = 5, pady = 5)
portField = Entry(serialFrame, width=30, borderwidth=3)
portField.insert(0, 'COM3')


# ==== define functions and buttons ====
# update log
def updateLog(text):
    log.configure(state ='normal')
    log.insert(INSERT, text + '\n')
    log.configure(state ='disabled')
    log.see(END)

# send command
def sendCommand(event=None):
    command = commandField.get()
    if command:
        teensy.write(bytes(command, 'utf-8'))
        updateLog('Command Sent: ' + command)
        commandField.delete(0, END)
button_sendCommand = Button(commandFrame, text='Send', command=sendCommand)
# bind the enter key to send command
root.bind('<Return>', sendCommand)

# read serial
def readSerial():
    data = str(teensy.readline())[2:-1]
    if data:
        monitor.configure(state ='normal')
        monitor.insert(INSERT, codecs.decode(data, "unicode_escape"))
        monitor.configure(state ='disabled')
        monitor.see(END)

# scan for serial ports
def scanPorts():
    availablePorts = list_ports.comports()
    updateLog('==== Found Serial Ports: ====')
    for port in availablePorts:
        updateLog('- ' + port.description)
button_scanPorts = Button(serialFrame, text='Scan For Serial Ports', command=scanPorts)
        
# update serial button
def updateSerialPort():
    teensy.port = portField.get()
button_updatePort = Button(serialFrame, text='Set Serial Port', command=updateSerialPort)


# ==== draw everything ====
# draw command frame
commandFrame.grid(row=0, column=0, padx=5, pady=5, sticky=W+E)
commandField.grid(row=0, column=0, padx=5, pady=3, sticky=W+E)
button_sendCommand.grid(row=0, column=1, sticky=W+E)

# log frame
logFrame.grid(row=1, column=0, padx=5, pady=5, sticky=N+S+E+W)
log.pack(expand=True, fill=BOTH)

# draw monitor frame
monitorFrame.grid(row=0, column=1, padx=5, pady=5, sticky=N+W+E+S, rowspan=3)
monitor.pack(expand=True, fill=BOTH)

# draw serial frame
serialFrame.grid(row=2, column=0, padx=5, pady=5, sticky=W+E)
portField.grid(row=0, column=0, sticky=W+E)
button_updatePort.grid(row=0, column=1, sticky=W+E)
button_scanPorts.grid(row=1, column=0, sticky=W+E, columnspan=2)


# ==== start application ====
teensy = serial.Serial(port='COM3', baudrate=9600, timeout=10/1000)
commandField.focus() # place cursor in command field

serialThread = threading.Thread(target=readSerial)

while True:
    readSerial()
    root.update()   
