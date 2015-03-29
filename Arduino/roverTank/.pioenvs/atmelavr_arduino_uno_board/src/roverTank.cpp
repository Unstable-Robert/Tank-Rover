#include <Arduino.h>
#include <SPI.h>
#include <Servo.h>
#include <Wire.h>
#include "Adafruit_BLE_UART.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_10DOF.h>

void setup(void);

void loop();
void findNextDirection();
long getRightDistance();

long getLeftDistance();

long getPingSensorDistance();

int getCurrentHeading();
int getLeftHeading(int heading);
int getRightHeading(int heading);

void handlingBT();
void setMotorSpeed(int left, int right);
void turnLeft(int heading);
void turnRight(int heading);
void reverseFor(int time);
void checkBTStatus();
#line 1 "roverTank.ino"
//#include <SPI.h>
//#include <Servo.h>
//#include <Wire.h>

//#include "Adafruit_BLE_UART.h"
//#include <Adafruit_Sensor.h>
//#include <Adafruit_LSM303_U.h>
//#include <Adafruit_L3GD20_U.h>
//#include <Adafruit_BMP085_U.h>
//#include <Adafruit_10DOF.h>

#define ADAFRUITBLE_REQ 10
#define ADAFRUITBLE_RDY 2
#define ADAFRUITBLE_RST 9

#define RIGHTMOTOR 4
#define LEFTMOTOR 7
#define PINGSENSOR 6
#define BLUELED 8
#define PINGSERVO 5

#define CENTER 85
#define LEFT 1
#define RIGHT 167

Adafruit_10DOF                dof   = Adafruit_10DOF();
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
Adafruit_LSM303_Mag_Unified   mag   = Adafruit_LSM303_Mag_Unified(30302);

Servo ps;
Servo rm;
Servo lm;

aci_evt_opcode_t status;
aci_evt_opcode_t laststatus = ACI_EVT_DISCONNECTED;
int myBytes[2];
unsigned long interval = 3000;
unsigned long currentMillis = millis();
unsigned long previousMillis;
boolean AIMode = true;

Adafruit_BLE_UART BTLEserial = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);

void setup(void) {
    Serial.begin(9600);
    while(!Serial);
    Serial.println(F("Adafruit Bluefruit Low Energy nRF8001 Print echo demo"));

    mag.begin();
    BTLEserial.begin();

    pinMode(BLUELED, OUTPUT);

    rm.attach(RIGHTMOTOR);
    lm.attach(LEFTMOTOR);
    ps.attach(PINGSERVO);
    ps.write(90);
    delay(3000);
}

void loop() {
    setMotorSpeed(1700, 1300);
    Serial.println(getCurrentHeading());
    /*currentMillis = millis();
    long distance = getPingSensorDistance();
    if (distance < 30) {
        digitalWrite(BLUELED, HIGH);
        while(getPingSensorDistance() < 40) {
            setMotorSpeed(1600, 1600);
            delay(30);
        }
        setMotorSpeed(1500, 1500);
        if (AIMode) findNextDirection();
    } else {
        if (AIMode) setMotorSpeed(1400, 1400);
        digitalWrite(BLUELED, LOW);
    }
    checkBTStatus();
    if (status == ACI_EVT_CONNECTED) {
        if (!AIMode) {
        handlingBT();
        } else {
        if (BTLEserial.available()){
            for (int x = 0; x < 2; x++) {
                myBytes[x] = BTLEserial.read();
                if (myBytes[x] != 0x96) AIMode = false;
                previousMillis = currentMillis;
            }

        }
    }
  }*/
  /*delay(20);*/
}
void findNextDirection() {
    long right = getRightDistance();
    Serial.print("Right: ");Serial.println(right);
    delay(500);
    long left = getLeftDistance();
    Serial.print("Left: ");Serial.println(left);
    if ((left > 30) || (right > 30)) {
        if (left > right) {
            turnLeft(getCurrentHeading());
        } else if (right > left) {
            turnRight(getCurrentHeading());
        } else {
            reverseFor(200);
        }
    } else {
        while(getPingSensorDistance() < 50) {
            setMotorSpeed(1600, 1600);
            delay(30);
        }
    }
}
long getRightDistance() {
    ps.write(RIGHT);
    delay(300);
    long distanceR = getPingSensorDistance();
    delay(100);
    ps.write(CENTER);
    return distanceR;
}

long getLeftDistance() {
    ps.write(LEFT);
    delay(300);
    long distanceL = getPingSensorDistance();
    delay(100);
    ps.write(CENTER);
    return distanceL;
}

long getPingSensorDistance() {
    pinMode(PINGSENSOR, OUTPUT);
    digitalWrite(PINGSENSOR, LOW);
    delayMicroseconds(2);
    digitalWrite(PINGSENSOR, HIGH);
    delayMicroseconds(5);
    pinMode(PINGSENSOR, INPUT);
    long duration = pulseIn(PINGSENSOR,HIGH);
    return duration / 29 / 2;
}

int getCurrentHeading() {
    sensors_event_t event;
    sensors_vec_t   orientation;
    mag.getEvent(&event);
    float Pi = 3.14159;
    // Calculate the angle of the vector y,x
    float heading = (atan2(event.magnetic.y,event.magnetic.x) * 180) / Pi;
    if (heading < 0) {
    heading = 360 + heading;
    }
    return (int)heading;
}
int getLeftHeading(int heading) {  //subtract 90
    int newHeading = heading - 90;
    while (newHeading < 0) {
        newHeading = newHeading + 360;
    }
    return newHeading;

}
int getRightHeading(int heading) { //add 90
    int newHeading = heading + 90;
    while (newHeading >= 360) {
        newHeading = newHeading - 360;
    }
    return newHeading;
}

void handlingBT() {
    if(currentMillis - previousMillis> interval) {
        previousMillis = currentMillis;
        interval = 3000;
        for (int x = 0; x < 2; x++) {
            myBytes[x] = 0x96;
        }
    }
    if (BTLEserial.available() == 2) {
        for (int x = 0; x < 2; x++) {
            myBytes[x] = BTLEserial.read();
            previousMillis = currentMillis;
        }
        setMotorSpeed(myBytes[0] * 10, myBytes[1] * 10);
    }
}
void setMotorSpeed(int left, int right) {
    lm.writeMicroseconds(left);
    rm.writeMicroseconds(right);
}
void turnLeft(int heading) {
    while((getCurrentHeading() > getLeftHeading(heading))){
        Serial.print("TURNINGHeading: ");Serial.println(getCurrentHeading());
        Serial.print("TurnLeftHeading: ");Serial.println(getLeftHeading(heading));
        if (getCurrentHeading() < getLeftHeading(heading) + 20) {
            Serial.println("TURNEDTOOO MUCHHHHHH");
        }
        setMotorSpeed(1700, 1300);
        /*delay(500);*/
    }
    setMotorSpeed(1500, 1500);
}
void turnRight(int heading) {
    while((getCurrentHeading() < getRightHeading(heading))){
        Serial.print("TURNINGHeading: ");Serial.println(getCurrentHeading());
        Serial.print("TurnRightHeading: ");Serial.println(getRightHeading(heading));
        setMotorSpeed(1300, 1700);
        /*delay(500);*/
    }
    setMotorSpeed(1500, 1500);
}
void reverseFor(int time) {
    setMotorSpeed(1700, 1700);
    delay(time);
    setMotorSpeed(1500, 1500);
}
void checkBTStatus() {
    BTLEserial.pollACI();
    status = BTLEserial.getState();
    if (status != laststatus) {
    if (status == ACI_EVT_CONNECTED) {
      previousMillis = currentMillis;
    }
    if (status == ACI_EVT_DISCONNECTED) {
      setMotorSpeed(1500, 1500);
    }
    laststatus = status;
    }
}
