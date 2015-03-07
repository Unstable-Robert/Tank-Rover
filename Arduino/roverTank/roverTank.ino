#include <SPI.h>
#include <Servo.h>
#include "Adafruit_BLE_UART.h"

#define ADAFRUITBLE_REQ 10
#define ADAFRUITBLE_RDY 2
#define ADAFRUITBLE_RST 9

#define RIGHTMOTOR 4
#define LEFTMOTOR 7
#define PINGSENSOR 6
#define BLUELED 8
#define PINGSERVO 5

#define CENTER 85
#define LEFT 167
#define RIGHT 1

Servo ps;
Servo rm;
Servo lm;

aci_evt_opcode_t status;
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
  
  BTLEserial.begin();
  pinMode(BLUELED, OUTPUT);
  
  rm.attach(RIGHTMOTOR);
  lm.attach(LEFTMOTOR);
  ps.attach(PINGSERVO);
  ps.write(90);
  delay(2000);
}

aci_evt_opcode_t laststatus = ACI_EVT_DISCONNECTED;

void loop() {
  currentMillis = millis();
  long distance = getPingSensorDistance();
  if (distance < 30) {
    digitalWrite(BLUELED, HIGH);
    while(getPingSensorDistance() < 40) {
      setMotorSpeed(1700, 1700);
      delay(30);
    }
    setMotorSpeed(1500, 1500);
    if (AIMode) findNextDirection();
  } else {
    if (AIMode) setMotorSpeed(1300, 1300);
    digitalWrite(BLUELED, LOW);
  }
  checkBTStatus();
  if (status == ACI_EVT_CONNECTED) {
    if (!AIMode) {
      handlingBT();
    }
    //add something here to check if user turned off ai mode via bluetooth
  }
  delay(20);
}
void findNextDirection() {
  long right = getRightDistance();
  Serial.print("Right: ");Serial.println(right);
  long left = getLeftDistance();
  Serial.print("Left: ");Serial.println(left);
  
  if ((left > 30) || (right > 30)) {
    if (left > right) {
      turnLeft();
    } else if (right > left) {
      turnRight();
    } else {
      reverseFor(300);
      findNextDirection();
    }
    
  } else {
    while(getPingSensorDistance() < 50) {
      setMotorSpeed(1700, 1700);
      delay(30);
    }
  }
}
long getRightDistance() {
  ps.write(1);
  long distanceR = getPingSensorDistance();
  ps.write(85);
  delay(20);
  return distanceR;
}

long getLeftDistance() {
  ps.write(167);
  long distanceL = getPingSensorDistance();
  ps.write(85);
  delay(20);
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
void turnLeft() {
  setMotorSpeed(1000, 2000);
  delay(500);
  setMotorSpeed(1500, 1500);
}
void turnRight() {
    setMotorSpeed(2000, 1000);
  delay(500);
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
