#include <SPI.h>
#include <Servo.h>
#include <Wire.h>

#include "Adafruit_BLE_UART.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_10DOF.h>

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
    pinMode(3, INPUT);
    rm.attach(RIGHTMOTOR);
    lm.attach(LEFTMOTOR);
    ps.attach(PINGSERVO);
    ps.write(CENTER);
    delay(3000);
    attachInterrupt(1, buttonPressed, RISING);
    Serial.print("Done with Setup");
}

void loop() {
  runningLoop();
  /*testTurning();*/
  /*testingCompass();*/
}
void buttonPressed() {
    Serial.println("ButtonPressed");
}
void testTurning() {
    turnLeft(getCurrentHeading());
    delay(1000);
    Serial.println("turning again");
    Serial.println(getCurrentHeading());
}
void testingCompass() {
    setMotorSpeed(1200, 1800);
    Serial.println(getCurrentHeading());     //values go from 0 - 359 outside of chasie
}
void runningLoop() {
    currentMillis = millis();
    long distance = getPingSensorDistance();
    if (distance < 30) {
        digitalWrite(BLUELED, HIGH);
        while(getPingSensorValue() < 40) {
            Serial.println("<30 Backing up");
            setMotorSpeed(1300, 1300);
            delay(30);
            /*setMotorSpeed(1500, 1500);*/
        }
        setMotorSpeed(1500, 1500);
        if (AIMode) findNextDirection();
    } else {
        if (AIMode) setMotorSpeed(1600, 1600);
        digitalWrite(BLUELED, LOW);
    }
    checkBTStatus();
    if (status == ACI_EVT_CONNECTED) {
        /*sendBTInfo(distance, getCurrentHeading());*/
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
  }
}
void sendBTInfo(long dist, int heading) {
    String send ="{" + String(heading) + ","+ String(dist) + "} ";
    /*Serial.println(send);
    Serial.println(send.length());*/
    uint8_t buf[send.length()];
    send.getBytes(buf, send.length());
    BTLEserial.write(buf, send.length());
}
void findNextDirection() {
    long right = getRightDistance();
    Serial.print("Right: ");Serial.println(right);
    delay(500);
    long left = getLeftDistance();
    Serial.print("Left: ");Serial.println(left);
    if ((left > 30) || (right > 30)) {
        if (left > right) {
            turnTo(getLeftHeading(getCurrentHeading()), LEFT);
            /*turnLeft(getCurrentHeading());*/
        } else if (right > left) {
            turnTo(getRightHeading(getCurrentHeading()), RIGHT);
            /*turnRight(getCurrentHeading());*/
        } else {
            while(getPingSensorDistance() < 50) {
                setMotorSpeed(1400, 1400);
                delay(30);
            }
        }
    } else {
        while(getPingSensorDistance() < 50) {
            setMotorSpeed(1400, 1400);
            delay(30);
        }
    }
}
void turnTo(int heading, int dir) {
    boolean turning = true;
    int delayTime = 200;
    while (turning) {
        if (dir == LEFT) {
            setMotorSpeed(1800, 1200);
            delay(delayTime);
            setMotorSpeed(1500, 1500);
        }
        if (dir == RIGHT) {
            setMotorSpeed(1200, 1800);
            delay(delayTime);
            setMotorSpeed(1500, 1500);
        }
        if (((heading - 10) <= getCurrentHeading()) && ((heading + 10) >= getCurrentHeading())) {
            delayTime = 100;
        }
        Serial.print("CurrentHeading: "); Serial.println(getCurrentHeading());
        Serial.print("Heading: "); Serial.println(heading);
        delay(300);
        if (((heading - 2) <= getCurrentHeading()) && ((heading + 2) >= getCurrentHeading())) {
            turning = false;
        }
    }
}
long getRightDistance() {
    ps.write(RIGHT);
    delay(300);
    long distanceR = getPingSensorDistance();
    delay(150);
    ps.write(CENTER);
    return distanceR;
}

long getLeftDistance() {
    ps.write(LEFT);
    delay(300);
    long distanceL = getPingSensorDistance();
    delay(150);
    ps.write(CENTER);
    return distanceL;
}

long getPingSensorDistance() {
    long val1 = getPingSensorValue();
    delay(300);
    long val2 = getPingSensorValue();
    delay(300);
    long val3 = getPingSensorValue();
    delay(300);
    long val4 = getPingSensorValue();
    delay(300);
    long com = val1 + val2 + val3 + val4;
    return com/4;
}
long getPingSensorValue() {
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
    if (dof.magGetOrientation(SENSOR_AXIS_Z, &event, &orientation)) {
      return orientation.heading;
    }
    return -1;
}
int getLeftHeading(int heading) {  //add 90
    int newHeading = heading + 90;
    while (newHeading >= 360) {
        newHeading = newHeading - 360;
    }
    return newHeading;
}
int getRightHeading(int heading) { //subtract 90
    int newHeading = heading - 90;
    while (newHeading < 0) {
        newHeading = newHeading + 360;
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
    long head = getCurrentHeading();
    long nextHead = getLeftHeading(heading);
    Serial.print("CurrentHeading: "); Serial.println(head);
    Serial.print("Heading: "); Serial.println(heading);
    Serial.print("TurningToo: "); Serial.println(nextHead);
    while(getCurrentHeading() < getLeftHeading(heading)){
        Serial.print("TURNINGHeading: ");Serial.println(getCurrentHeading());
        Serial.print("TurnLeftHeading: ");Serial.println(getLeftHeading(heading));

        /*delay(10);
        setMotorSpeed(1500, 1500);*/
        /*delay(200);*/
    }
    setMotorSpeed(1500, 1500);
}
void turnRight(int heading) {
    while((getCurrentHeading() > getRightHeading(heading))){
        Serial.print("TURNINGHeading: ");Serial.println(getCurrentHeading());
        Serial.print("TurnRightHeading: ");Serial.println(getRightHeading(heading));
        setMotorSpeed(1800, 1200);
        /*delay(10);
        setMotorSpeed(1500, 1500);*/
        /*delay(200);*/
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
