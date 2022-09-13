import threading
import socket
from time import sleep
import enum

class MOTOR(enum.Enum):
    LEFT = 0
    RIGHT = 1

class ControllerThread:
    def ConnectToCutter(self, host, port):
        pass
    def SendNewMotorSpeed(self, motor, speed):
        pass
    def CloseConnection(self):
        pass
    def SendMessage(self, msg):
        pass
    def ControllerLoop(self):
        pass

