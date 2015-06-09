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
long lastDebounceTime = 0;
long debounceDelay = 100;
boolean AIMode = true;

Adafruit_BLE_UART BTLEserial = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);

void setup(void) {
    attachInterrupt(1, buttonPressed, FALLING);
    Serial.begin(9600);
    while(!Serial);
    Serial.println();
    startingUpCommand("Up");
    startingUpCommand("Swagger");
    mag.begin();
    accel.begin();
    startingUpCommand("Dooms Day Weapon");
    BTLEserial.begin();
    pinMode(BLUELED, OUTPUT);
    startingUpCommand("Emotions");
    rm.attach(RIGHTMOTOR);
    lm.attach(LEFTMOTOR);
    ps.attach(PINGSERVO);
    ps.write(CENTER);
    startingUpCommand("Facial Reconition");
    Serial.print("Done with Setup");
    delay(1000);
}
void startingUpCommand(String process) {
    Serial.print("Booting " + process + "..");
    int length = process.length() + 10;
    for (int x = length; x < 35; x++) {
        Serial.print(".");
        delay(250);
    }
    Serial.println();
}

void loop() {
  runningLoop();
  /*testTurning();*/
  /*testingCompass();*/
  /*rectangle();
  delay(2000);*/
}
void rectangle() {
    /*setMotorSpeed(1700, 1700);
    delay(1000);
    setMotorSpeed(1500, 1500);*/
    turnTo(136, RIGHT);
    delay(1000);
    turnTo(237, RIGHT);
    delay(1000);
    turnTo(306, RIGHT);
    delay(1000);
    turnTo(10, RIGHT);
    delay(1000);
}
void testTurning() {
    turnTo(getLeftHeading(getCurrentHeading()), LEFT);
    delay(1000);
    Serial.println("turning again");
    Serial.println(getCurrentHeading());
}
void testingCompass() {
    sensors_event_t event;
    sensors_vec_t   orientation;
    mag.getEvent(&event);
    Serial.println("MAG");
    if (dof.magGetOrientation(SENSOR_AXIS_Z, &event, &orientation)) {
      Serial.print("HeadingZ: ");Serial.println(orientation.heading);
    }
    Serial.print("compensated HeadingZ: ");Serial.println(getCurrentHeading());
    if (dof.magGetOrientation(SENSOR_AXIS_Y, &event, &orientation)) {
      Serial.print("HeadingY: ");Serial.println(orientation.heading);
    }
    if (dof.magGetOrientation(SENSOR_AXIS_X, &event, &orientation)) {
      Serial.print("HeadingX: ");Serial.println(orientation.heading);
    }
    Serial.print("X: "); Serial.print(event.magnetic.x); Serial.print("  ");
    Serial.print("Y: "); Serial.print(event.magnetic.y); Serial.print("  ");
    Serial.print("Z: "); Serial.print(event.magnetic.z); Serial.print("  ");Serial.println("uT");
    delay(2000);
}
void runningLoop() {
    aiLED();
    currentMillis = millis();
    long distance = getPingSensorValue();
    Serial.print("Distance: "); Serial.println(distance);
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
        if (AIMode) setMotorSpeed(1700, 1700);
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
            if (!AIMode) handlingBT();

        }
    }
  }
}
void buttonPressed() {
    if (millis() - lastDebounceTime > debounceDelay) {
        AIMode = !AIMode;
        Serial.print("ButtonPressed");Serial.println(AIMode);
    }
    lastDebounceTime = millis();
    aiLED();
}
void aiLED() {
    if (AIMode) {
        digitalWrite(BLUELED, HIGH);
    } else {
        digitalWrite(BLUELED, LOW);
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
        } else if (right > left) {
            turnTo(getRightHeading(getCurrentHeading()), RIGHT);
        } else {
            reverseFor(300);
            findNextDirection();
        }
    } else {
        reverseFor(300);
        findNextDirection();
    }
    if (!AIMode) runningLoop();
}
void turnTo(int heading, int dir) {
    boolean turning = true;
    int delayTime = 200;
    while (turning) {
        if (dir == LEFT) {
            setMotorSpeed(1100, 1800);
            delay(delayTime);
            setMotorSpeed(1500, 1500);
        }
        if (dir == RIGHT) {
            setMotorSpeed(1800, 1200);
            delay(delayTime);
            setMotorSpeed(1500, 1500);
        }
        delay(800);
        if ((dir == LEFT) && (getCurrentHeading() < heading) && (delayTime == 100)) {
            dir = RIGHT;
            delayTime = 50;
        }
        if ((dir == RIGHT) && (getCurrentHeading() > heading) && (delayTime == 100)) {
            dir = LEFT;
            delayTime = 50;
        }
        if (((heading - 10) <= getCurrentHeading()) && ((heading + 10) >= getCurrentHeading())) {
            delayTime = 100;
        }
        Serial.print("CurrentHeading: "); Serial.println(getCurrentHeading());
        Serial.print("Heading: "); Serial.println(heading);
        if (((heading - 1) <= getCurrentHeading()) && ((heading + 1) >= getCurrentHeading())) {
            turning = false;
        }
        if (!AIMode) runningLoop();
    }
}
long getRightDistance() {
    ps.write(RIGHT);
    delay(300);
    long distanceR = getPingSensorDistance();
    delay(150);
    ps.write(CENTER);
    if (!AIMode) runningLoop();
    return distanceR;
}

long getLeftDistance() {
    ps.write(LEFT);
    delay(300);
    long distanceL = getPingSensorDistance();
    delay(150);
    ps.write(CENTER);
    if (!AIMode) runningLoop();
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
/*int getCurrentHeading() {
    sensors_event_t event;
    sensors_vec_t   orientation;
    mag.getEvent(&event);
    if (dof.magGetOrientation(SENSOR_AXIS_Z, &event, &orientation)) {
      return orientation.heading;
    }
    return -1;
}*/
int getCurrentHeading() {
    sensors_event_t mag_event;
    sensors_event_t accel_event;
    sensors_vec_t   orientation;
    mag.getEvent(&mag_event);
    accel.getEvent(&accel_event);
    if (dof.magTiltCompensation(SENSOR_AXIS_Z, &mag_event, &accel_event)) {
        if (dof.magGetOrientation(SENSOR_AXIS_Z, &mag_event, &orientation)) {
          return orientation.heading;
        }
        return -1;
    } else {
        return -1;
    }
}
int getLeftHeading(int heading) {  //subtract 90
    int newHeading = heading - 90;
    while (newHeading < 0) {
        newHeading = newHeading + 360;
    }
    if (!AIMode) runningLoop();
    return newHeading;
}
int getRightHeading(int heading) { //add 90
    int newHeading = heading + 90;
    while (newHeading >= 360) {
        newHeading = newHeading - 360;
    }
    if (!AIMode) runningLoop();
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
