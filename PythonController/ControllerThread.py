import threading
import socket
from time import sleep
import enum


class MOTOR(enum.Enum):
    LEFT = 0
    RIGHT = 1


class ControllerThread:
    def __int__(self, cutter_host, cutter_port):
        self.jarvisCutter = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.host = cutter_host
        self.port = cutter_port
        self.isconnected = False
        self.serverkey = ""
        self.returnkey = "RET"

    def connecttocutter(self):
        try:
            self.jarvisCutter.connect((self.host, self.port))
            print(f"Connected to {self.host}:{self.port}....")
            return self.getkeyfromcutter()
        except socket.error:
            return 3

    def getkeyfromcutter(self):
        self.isconnected = True
        self.serverkey = self.jarvisCutter.recv(3).decode()
        if self.serverkey == "KEY":
            print(self.serverkey)
            self.jarvisCutter.send(self.returnkey.encode())
            return 0
        else:
            return 2

    def sendnewmotorspeeds(self, leftspeed, rightspeed):
        try:
            msg = "{:0.1f}".format(leftspeed)
            msg = msg + "|" + "{:0.1f}".format(rightspeed)
            self.jarvisCutter.send(msg.encode())
            return 0
        except socket.error:
            return 7

    def CloseConnection(self):
        pass

    def SendMessage(self, msg):
        pass

    def ControllerLoop(self):
        pass
