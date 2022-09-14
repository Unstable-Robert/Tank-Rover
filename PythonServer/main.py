# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.
from gpiozero import LED, Servo, Device
from time import sleep
import socket
from MotorController import MotorController


# main loop start stop command
JarvisCutter_Is_Running = False
JarvisRemote_Networking_Running = True

#handle network connections
remoteControlSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
remoteControlSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
host = "JCutter.local"
port = 24564
remoteControlSocket.bind((host, port))

motorController = MotorController(12, 13)


while JarvisRemote_Networking_Running:
    print(f"Waiting for connections for remote on {host}:{port}......")

    remoteControlSocket.listen(5)
    JarvisCutter_Is_Running = True
    while JarvisCutter_Is_Running:
        #do we get new commands
        controller, cAddr = remoteControlSocket.accept()
        print(f"Connection from {cAddr}")
        controller.send("KEY".encode())
        messageback = controller.recv(11).decode()
        print(messageback)
        isConnected = True
        while isConnected:
            data = controller.recv(11).decode()
            if (data == ""):
                motorController.stop_motors()
                controller.close()
                isConnected = False
                JarvisCutter_Is_Running = False
                break
            motor1Speed, motor2Speed = data.split("|")
            leftmotor = float(motor1Speed)
            rightmotor = float(motor2Speed)
            print(f"new Speed left:{leftmotor} and right:{rightmotor}")
            motorController.set_left_motor_speed(leftmotor)
            motorController.set_right_motor_speed(rightmotor)
            #motorController.stop_motors()
            motorController.update_motor_speeds()


        print(f"Connection Closed...")
# See PyCharm help at https://www.jetbrains.com/help/pycharm/
