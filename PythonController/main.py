# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.
from tkinter import *
from time import sleep
import socket
from ControllerThread import ControllerThread

# creating window

window = Tk()
window.title("Jarvis Cutter Remote")
window.minsize(width=500, height=300)
window.config(padx=10, pady=10)

window.mainloop()

host = "JCutter"
port = 24564

controllerthread = ControllerThread(host, port)

try:
    controllerthread.connecttocutter()

    while controllerthread.isconnected:
        controllerthread.sendnewmotorspeeds(0.0, 0.0)
        sleep(1)
except socket.error:
    print(f"got a socket error")
else:
    print(f"Something else went wrong")

# See PyCharm help at https://www.jetbrains.com/help/pycharm/
