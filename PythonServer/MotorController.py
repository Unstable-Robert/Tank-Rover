from gpiozero import Servo, LED
from gpiozero.pins.pigpio import PiGPIOFactory
from time import sleep


class MotorController:
    def __init__(self, leftmotorpin, rightmotorpin):
        self.pinFactory = PiGPIOFactory()
        self.leftMotorPin = leftmotorpin
        self.rightMotorPin = rightmotorpin
        self.leftMotorServo = Servo(leftmotorpin, pin_factory=self.pinFactory)
        self.rightMotorServo = Servo(rightmotorpin, pin_factory=self.pinFactory)
        self.lSpeed = 0.0
        self.rSPeed = 0.0

    def stop_motors(self):
        self.lSpeed = 0.0
        self.rSPeed = 0.0
        self.update_motor_speeds()

    def set_left_motor_speed(self, speed):
        self.lSpeed = speed

    def set_right_motor_speed(self, speed):
        self.rSPeed = speed

    def update_motor_speeds(self):
        self.leftMotorServo.value = self.lSpeed
        self.rightMotorServo.value = self.rSPeed
