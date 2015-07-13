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
long debounceDelay = 300;
boolean AIMode = true;

Adafruit_BLE_UART BTLEserial = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);

void setup(void) {
    attachInterrupt(1, buttonPressed, FALLING);
    Serial.begin(9600);
    while(!Serial);
    Serial.println();
    startingUpCommand("Up");
    startingUpCommand("Swagger");
    // mag.begin();
    // accel.begin();
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
  /*runningLoop();*/
  // testTurning();
  // testingCompass();
  // testingCompass2();
  // rectangle();
  // delay(2000);
}
void rectangle() {
    /*setMotorSpeed(1700, 1700);
    delay(1000);
    setMotorSpeed(1500, 1500);*/
    turnTo(136);
    delay(1000);
    turnTo(237);
    delay(1000);
    turnTo(306);
    delay(1000);
    turnTo(10);
    delay(1000);
}
void testTurning() {
    turnTo(getLeftHeading(getCurrentHeading()));
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    digitalWrite(BLUELED, HIGH);
    delay(2000);
    digitalWrite(BLUELED, LOW);
    Serial.print("turning again.....Heading:");Serial.println(getCurrentHeading());
}
void testingCompass2(){
    int currentHeading = getAvgHeading();
    Serial.print("CurrentHeading: "); Serial.println(currentHeading);
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
void sendBTInfo(int dist, int heading) {
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
            turnTo(getLeftHeading(getCurrentHeading()));
        } else if (right > left) {
            turnTo(getRightHeading(getCurrentHeading()));
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
void turnTo(int heading) {
    int dir = checkTurnDirection(getAvgHeading(), heading);
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
        delay(1000);
        int currentHeading = getAvgHeading();
        Serial.print("CurrentHeading: "); Serial.println(currentHeading);
        Serial.print("Heading: "); Serial.println(heading);
        dir = checkTurnDirection(currentHeading, heading);
        if (((heading - 10) <= currentHeading) && ((heading + 10) >= currentHeading)) {
            delayTime = 100;
        }
        if (((heading - 1) <= currentHeading) && ((heading + 1) >= currentHeading)) {
            Serial.print("Turn Complete....Heading: ");Serial.println(currentHeading);
            turning = false;
        }
        if (!AIMode) runningLoop();
    }
}
//ch = CurrentHeading
//t2 = TurnTo
int checkTurnDirection(int ch, int t2){
    if ((t2 > ch) && ((t2 - ch) <= 180)) {
        return RIGHT;
    } else if ((t2 > ch) && ((t2 - ch) > 180)) {
        return LEFT;
    } else if ((ch > t2) && ((ch - t2) <= 180)) {
        return LEFT;
    } else if ((ch > t2) && ((ch - t2) > 180)) {
        return RIGHT;
    } else {
        Serial.println("BROKEN");
        return 0;
    }
}
int getRightDistance() {
    ps.write(RIGHT);
    delay(300);
    int distanceR = getPingSensorDistance();
    delay(150);
    ps.write(CENTER);
    if (!AIMode) runningLoop();
    return distanceR;
}

int getLeftDistance() {
    ps.write(LEFT);
    delay(300);
    int distanceL = getPingSensorDistance();
    delay(150);
    ps.write(CENTER);
    if (!AIMode) runningLoop();
    return distanceL;
}

int getPingSensorDistance() {
    int val1 = getPingSensorValue();
    delay(300);
    int val2 = getPingSensorValue();
    delay(300);
    int val3 = getPingSensorValue();
    delay(300);
    int val4 = getPingSensorValue();
    delay(300);
    int com = val1 + val2 + val3 + val4;
    return com/4;
}
int getPingSensorValue() {
    pinMode(PINGSENSOR, OUTPUT);
    digitalWrite(PINGSENSOR, LOW);
    delayMicroseconds(2);
    digitalWrite(PINGSENSOR, HIGH);
    delayMicroseconds(5);
    pinMode(PINGSENSOR, INPUT);
    long duration = pulseIn(PINGSENSOR,HIGH);
    int dur = (int)duration / 29 / 2;
    return dur;
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
int getAvgHeading() {
    int myHeadings[5];
    int avg;
    for (int x = 0; x < 5; x++) {
        myHeadings[x] = getCurrentHeading();
        delay(200);
    }
    Serial.print("Headings: {");
    for (int x = 0; x < 4; x++) {
        Serial.print(myHeadings[x]);Serial.print(", ");
    }
    Serial.print(myHeadings[4]);Serial.println("}");
    for (int x = 0; x < 5; x++) {
        avg = avg + myHeadings[x];
    }
    return avg / 5;
}
void printAccelInfo() {
    sensors_event_t accel_event;
    accel.getEvent(&accel_event);
    Serial.print("X: "); Serial.print(accel_event.acceleration.x); Serial.print("  ");
    Serial.print("Y: "); Serial.print(accel_event.acceleration.y); Serial.print("  ");
    Serial.print("Z: "); Serial.print(accel_event.acceleration.z); Serial.println("  ");
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
