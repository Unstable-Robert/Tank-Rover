#include <SPI.h>
#include <Servo.h>
#include <Wire.h>

#include "Adafruit_BLE_UART.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_10DOF.h>

#include <NewPing.h>
#include <MahonyAHRS.h>

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
#define BROKEN 100

#define FRONT 165
#define RIGHTY 240
#define BACK 292
#define LEFTY 350

Adafruit_10DOF                dof   = Adafruit_10DOF();
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
Adafruit_LSM303_Mag_Unified   mag   = Adafruit_LSM303_Mag_Unified(30302);
Adafruit_L3GD20_Unified       gyro  = Adafruit_L3GD20_Unified(20);
Servo ps;
Servo rm;
Servo lm;

// Offsets applied to raw x/y/z values
float mag_offsets[3]            = { 3.36F, -13.07F, 1.25F };

// Soft iron error compensation matrix
float mag_softiron_matrix[3][3] = { { 0.999, 0.004, 0.009 },
                                    { 0.004, 1.020, -0.003 },
                                    { 0.009, -0.003, 0.982 } }; 

float mag_field_strength        = 52.49F;

// Mahony is lighter weight as a filter and should be used
// on slower systems
Mahony filter;


NewPing pingSensor(PINGSENSOR, PINGSENSOR, 1000);

aci_evt_opcode_t status;
aci_evt_opcode_t laststatus = ACI_EVT_DISCONNECTED;
int myBytes[2];
unsigned long interval = 3000;
unsigned long updateInterval = 1500;
unsigned long prevMillis;
unsigned long currentMillis = millis();
unsigned long previousMillis;
long lastDebounceTime = 0;
long debounceDelay = 300;
boolean AIMode = false;
boolean aiEnabled = false;

Adafruit_BLE_UART BTLEserial = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);

void setup(void) {
    digitalWrite(BLUELED, HIGH);
    attachInterrupt(1, buttonPressed, FALLING);
    Serial.begin(9600);
    while(!Serial);
    digitalWrite(BLUELED, LOW);
    delay(10);
    digitalWrite(BLUELED, HIGH);
    Serial.println();
    startingUpCommand("Up");
    startingUpCommand("Swagger");
    digitalWrite(BLUELED, LOW);
    delay(10);
    digitalWrite(BLUELED, HIGH);
    mag.begin();
    accel.begin();
    gyro.begin();
    startingUpCommand("Dooms Day Weapon");
    BTLEserial.begin();
    //BTLEserial.debugMode = false;
    pinMode(BLUELED, OUTPUT);
    startingUpCommand("Emotions");
    rm.attach(RIGHTMOTOR);
    lm.attach(LEFTMOTOR);
    ps.attach(PINGSERVO);
    ps.write(CENTER);
    digitalWrite(BLUELED, LOW);
    delay(10);
    digitalWrite(BLUELED, HIGH);
    startingUpCommand("Facial Reconition");
    Serial.print("Done with Setup");
    delay(1000);
    digitalWrite(BLUELED, LOW);
    delay(10);
    digitalWrite(BLUELED, HIGH);
    digitalWrite(BLUELED, LOW);
    delay(10);
    digitalWrite(BLUELED, HIGH);
}
void startingUpCommand(String process) {
    Serial.print("Booting " + process + "..");
    for (int x = process.length(); x < 25; x++) {
        Serial.print(".");
        delay(250);
    }
    Serial.println();
}

void loop() {
    // bostain();
   runningLoop();
  // testingPing();
  // testTurning();
  // testingCompass();
  // testingCompass2();
  // rectangle();
  // delay(2000);
}
void testingPing() {
    // NewPing sonar(PINGSENSOR, PINGSENSOR, 1000);
    // long dis = sonar.ping_median(5);
    // Serial.print("Time:");Serial.println(dis);
    // Serial.print("Distance:");Serial.println(sonar.convert_cm(dis));
    Serial.println(pingSensor.ping_cm());
    delay(150);
}
void bostain() {
    int heading = getCurrentHeading();
    if (!(((315 - 4) <= heading) && ((315 + 4) >= heading))) {
        turnTo(315);
        digitalWrite(BLUELED, HIGH);
        delay(3000);
        digitalWrite(BLUELED, LOW);
    }
    if (!(((193 - 4) <= heading) && ((193 + 4) >= heading))) {
        turnTo(93);
        digitalWrite(BLUELED, HIGH);
        delay(3000);
        digitalWrite(BLUELED, LOW);
    }
    // if (!(((FRONT - 4) <= heading) && ((FRONT + 4) >= heading))) {
    //     turnTo(FRONT);
    //     digitalWrite(BLUELED, HIGH);
    //     delay(3000);
    //     digitalWrite(BLUELED, LOW);
    // }
    // if (!(((205 - 4) <= heading) && ((205 + 4) >= heading))) {
    //     turnTo(205);
    //     digitalWrite(BLUELED, HIGH);
    //     delay(3000);
    //     digitalWrite(BLUELED, LOW);
    // }
    // if (!(((RIGHTY - 4) <= heading) && ((RIGHTY + 4) >= heading))) {
    //     turnTo(RIGHTY);
    //     digitalWrite(BLUELED, HIGH);
    //     delay(3000);
    //     digitalWrite(BLUELED, LOW);
    // }
    // if (!(((267 - 4) <= heading) && ((267 + 4) >= heading))) {
    //     turnTo(267);
    //     digitalWrite(BLUELED, HIGH);
    //     delay(3000);
    //     digitalWrite(BLUELED, LOW);
    // }
    // if (!(((BACK - 4) <= heading) && ((BACK + 4) >= heading))) {
    //     turnTo(BACK);
    //     digitalWrite(BLUELED, HIGH);
    //     delay(3000);
    //     digitalWrite(BLUELED, LOW);
    // }
    // if (!(((325 - 4) <= heading) && ((325 + 4) >= heading))) {
    //     turnTo(325);
    //     digitalWrite(BLUELED, HIGH);
    //     delay(3000);
    //     digitalWrite(BLUELED, LOW);
    // }
    // if (!(((LEFTY - 4) <= heading) && ((LEFTY + 4) >= heading))) {
    //     turnTo(LEFTY);
    //     digitalWrite(BLUELED, HIGH);
    //     delay(3000);
    //     digitalWrite(BLUELED, LOW);
    // }
    // if (!(((66 - 4) <= heading) && ((66 + 4) >= heading))) {
    //     turnTo(66);
    //     digitalWrite(BLUELED, HIGH);
    //     delay(3000);
    //     digitalWrite(BLUELED, LOW);
    // }
}
void rectangle() {
    turnTo(343);
    setMotorSpeed(1700, 1700);
    delay(1000);
    setMotorSpeed(1500, 1500);
    turnTo(136);
    setMotorSpeed(1700, 1700);
    delay(1000);
    setMotorSpeed(1500, 1500);
    turnTo(230);
    setMotorSpeed(1700, 1700);
    delay(1000);
    setMotorSpeed(1500, 1500);
    turnTo(285);
    setMotorSpeed(1700, 1700);
    delay(1000);
    setMotorSpeed(1500, 1500);
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
    int currentHeading = getCurrentHeading();
    Serial.print("CurrentHeading: "); Serial.println(currentHeading);
    delay(500);
    checkBTStatus();
    if (status == ACI_EVT_CONNECTED) {
        sendBTInfo(getPingSensorValue(), getCurrentHeading());
    }
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
    if (distance < 30) {
        digitalWrite(BLUELED, HIGH);
        while(getPingSensorValue() < 40) {
            Serial.print("<30 Backing up - ");Serial.println(distance);
            setMotorSpeed(1700, 1700);
            delay(30);
        }
        setMotorSpeed(1500, 1500);
        if (AIMode && aiEnabled) findNextDirection();
    } else {
        if (AIMode && aiEnabled) setMotorSpeed(1700, 1700);
    }
    checkBTStatus();
    if (status == ACI_EVT_CONNECTED) {
        sendBTInfo(distance, getCurrentHeading());
        if (!AIMode || !aiEnabled) {
            handlingBT();
        } else {
            if (BTLEserial.available()){
                for (int x = 0; x < 2; x++) {
                    myBytes[x] = BTLEserial.read();
                    if (myBytes[x] != 0x96) AIMode = false;
                    previousMillis = currentMillis;
                }
                if (!AIMode && !aiEnabled) handlingBT();
        }
    }
  }
}
void buttonPressed() {
    if (millis() - lastDebounceTime > debounceDelay) {
        if (aiEnabled) {
            AIMode = !AIMode;
            Serial.print("ButtonPressed");Serial.println(AIMode);
        }
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
    if((currentMillis - prevMillis) > updateInterval) {
        prevMillis = currentMillis;
        //Serial.println("here");
        String send ="{" + String(heading) + ","+ String(dist) + "} ";
        uint8_t buf[send.length()];
        send.getBytes(buf, send.length());
        BTLEserial.write(buf, send.length());
    }
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
    if (!AIMode) loop();
}
void turnTo(int heading) {
    int dir = checkTurnDirection(getAvgHeading(), heading);
    boolean turning = true;
    int delayTime = 600;
    while (turning) {
        if (dir == LEFT) {
            setMotorSpeed(1200, 1800);
            delay(delayTime);
            setMotorSpeed(1500, 1500);
        }
        if (dir == RIGHT) {
            setMotorSpeed(1800, 1200);
            delay(delayTime);
            setMotorSpeed(1500, 1500);
        }
        if (dir == BROKEN) {
            turning = false;
        }
        delay(1000);
        int currentHeading = getAvgHeading();
        Serial.print("CurrentHeading: "); Serial.println(currentHeading);
        Serial.print("Heading: "); Serial.println(heading);
        dir = checkTurnDirection(currentHeading, heading);
        if (((heading - 10) <= currentHeading) && ((heading + 10) >= currentHeading)) {
            delayTime = 100;
        }
        if (((heading - 2) <= currentHeading) && ((heading + 2) >= currentHeading)) {
            Serial.print("Turn Complete....Heading: ");Serial.println(currentHeading);
            // setMotorSpeed(1500, 1500);
            turning = false;
        }
        if (!AIMode) {
            turning = false;
        }
        checkBTStatus();
        if (status == ACI_EVT_CONNECTED) {
            sendBTInfo(getPingSensorValue(), currentHeading);
        }
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
        return BROKEN;
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
    // int val1 = getPingSensorValue();
    // delay(300);
    // int val2 = getPingSensorValue();
    // delay(300);
    // int val3 = getPingSensorValue();
    // delay(300);
    // int val4 = getPingSensorValue();
    // delay(300);
    // int com = val1 + val2 + val3 + val4;
    // return com/4;
    long dis = pingSensor.ping_median(5);
    return pingSensor.convert_cm(dis);
}
int getPingSensorValue() {
    // pinMode(PINGSENSOR, OUTPUT);
    // digitalWrite(PINGSENSOR, LOW);
    // delayMicroseconds(2);
    // digitalWrite(PINGSENSOR, HIGH);
    // delayMicroseconds(5);
    // pinMode(PINGSENSOR, INPUT);
    // long duration = pulseIn(PINGSENSOR,HIGH);
    // int dur = (int)duration / 29 / 2;
    // Serial.println(duration);
    int ping = pingSensor.ping_cm();
    //Serial.print("TestPing:");Serial.println(ping);
    return ping;
}
/* Not calibrated
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
}*/
int getCurrentHeading() {
    sensors_event_t gyro_event;
    sensors_event_t accel_event;
    sensors_event_t mag_event;

    gyro.getEvent(&gyro_event);
    accel.getEvent(&accel_event);
    mag.getEvent(&mag_event);
    // Apply mag offset compensation (base values in uTesla)
    float x = mag_event.magnetic.x - mag_offsets[0];
    float y = mag_event.magnetic.y - mag_offsets[1];
    float z = mag_event.magnetic.z - mag_offsets[2];

    // Apply mag soft iron error compensation
    float mx = x * mag_softiron_matrix[0][0] + y * mag_softiron_matrix[0][1] + z * mag_softiron_matrix[0][2];
    float my = x * mag_softiron_matrix[1][0] + y * mag_softiron_matrix[1][1] + z * mag_softiron_matrix[1][2];
    float mz = x * mag_softiron_matrix[2][0] + y * mag_softiron_matrix[2][1] + z * mag_softiron_matrix[2][2];

    // The filter library expects gyro data in degrees/s, but adafruit sensor
    // uses rad/s so we need to convert them first (or adapt the filter lib
    // where they are being converted)
    float gx = gyro_event.gyro.x * 57.2958F;
    float gy = gyro_event.gyro.y * 57.2958F;
    float gz = gyro_event.gyro.z * 57.2958F;

    // Update the filter
    filter.update(gx, gy, gz,
                    accel_event.acceleration.x, accel_event.acceleration.y, accel_event.acceleration.z,
                    mx, my, mz);

    float heading = filter.getYaw();
    Serial.print("Current Heading:");Serial.println(heading);
    return heading;
}
int getAvgHeading() {
    return getCurrentHeading();
    int arraySize = 2;
    int myHeadings[arraySize];
    int avg = 0;
    for (int x = 0; x < arraySize; x++) {
        myHeadings[x] = getCurrentHeading();
        delay(50);
    }
    Serial.print("Headings: {");
    for (int x = 0; x < arraySize - 1; x++) {
        Serial.print(myHeadings[x]);Serial.print(", ");
    }
    Serial.print(myHeadings[arraySize - 1]);Serial.println("}");
    for (int x = 0; x < arraySize; x++) {
        avg = avg + myHeadings[x];
    }
    avg = avg / arraySize;
    return avg;
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
    Serial.println("Got Message");
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
    //Serial.println(status);
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
