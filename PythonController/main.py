# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.
from tkinter import *
from time import sleep
import socket


#creating window

window = Tk()
window.title("Jarvis Cutter Remote")
window.minsize(width=500, height=300)
window.config(padx=10, pady=10)

window.mainloop()


jarvisCutter = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

host = "JCutter"
port = 24564

try:
    jarvisCutter.connect((host, port))
    print(f"Connected to {host}:{port}....")
    isConnected = True
    key = jarvisCutter.recv(3).decode()
    print(key)
    jarvisCutter.send("RET".encode())
    while isConnected:
        jarvisCutter.send("-0.9|-0.9".encode())
        sleep(1)
except socket.error:
    print(f"got a socket error")
else:
    print(f"Something else went wrong")


# See PyCharm help at https://www.jetbrains.com/help/pycharm/
