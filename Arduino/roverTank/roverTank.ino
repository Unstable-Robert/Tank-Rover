#include <SPI.h>
#include <Servo.h>
#include "Adafruit_BLE_UART.h"

#define ADAFRUITBLE_REQ 10
#define ADAFRUITBLE_RDY 2
#define ADAFRUITBLE_RST 9

//#define MotorOnePin4 8
//#define MotorOnePin3 7
//#define MotorOneSped 6
//
//#define MotorTwoPin2 4
//#define MotorTwoPin1 3
//#define MotorTwoSped 5

Servo rm;
Servo lm;

Adafruit_BLE_UART BTLEserial = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);

void setup(void) {
  Serial.begin(9600);
  while(!Serial);
  Serial.println(F("Adafruit Bluefruit Low Energy nRF8001 Print echo demo"));

  //BTLEserial.setDeviceName("TANK!!"); /* 7 characters max! */

  BTLEserial.begin();
//  pinMode(MotorOnePin4, OUTPUT);
//  pinMode(MotorOnePin3, OUTPUT);
//  pinMode(MotorOneSped, OUTPUT);
//
//  pinMode(MotorTwoPin2, OUTPUT);
//  pinMode(MotorTwoPin1, OUTPUT);
//  pinMode(MotorTwoSped, OUTPUT);
  
  rm.attach(7);
  lm.attach(4);

}

aci_evt_opcode_t laststatus = ACI_EVT_DISCONNECTED;

void loop() {
  unsigned long currentMillis = millis();
  unsigned long previousMillis;
  unsigned long interval = 3000;
  BTLEserial.pollACI();
  aci_evt_opcode_t status = BTLEserial.getState();
  if (status != laststatus) {
    if (status == ACI_EVT_DEVICE_STARTED) {
//        Serial.println(F("* Advertising started"));
    }
    if (status == ACI_EVT_CONNECTED) {
//        Serial.println(F("* Connected!"));
    }
    if (status == ACI_EVT_DISCONNECTED) {
//        Serial.println(F("* Disconnected or advertising timed out"));
//        digitalWrite(MotorTwoPin2, LOW);
//        digitalWrite(MotorTwoPin1, LOW);
//        digitalWrite(MotorOnePin4, LOW);
//        digitalWrite(MotorOnePin3, LOW);
      lm.writeMicroseconds(1500);
//      delay(200);
      rm.writeMicroseconds(1500);
//      delay(200);

    }
    laststatus = status;
  }

  if (status == ACI_EVT_CONNECTED) {
    int myBytes[4];
    if (BTLEserial.available()) {
        for (int x = 0; x < 4; x++) {
          myBytes[x] = BTLEserial.read();
          previousMillis = currentMillis;
        }
        if (myBytes[0] == 0x00) {
//          digitalWrite(MotorOnePin4, LOW);
//          digitalWrite(MotorOnePin3, LOW);
          lm.writeMicroseconds(1500);
//          delay(200);
        }
        if (myBytes[0] == 0x01) {
//          digitalWrite(MotorOnePin4, HIGH);
//          digitalWrite(MotorOnePin3, LOW);
          lm.writeMicroseconds(2000);
//          delay(200);
        }
        if (myBytes[0] == 0x02) {
//          digitalWrite(MotorOnePin4, LOW);
//          digitalWrite(MotorOnePin3, HIGH);
          lm.writeMicroseconds(1000);
//          delay(200);
        }
        if (myBytes[1] != 0x00) {
//          analogWrite(MotorOneSped, myBytes[1]);
        }
        if (myBytes[2]  == 0x00) {
//          digitalWrite(MotorTwoPin2, LOW);
//          digitalWrite(MotorTwoPin1, LOW);
          rm.writeMicroseconds(1500);
//          delay(200);
        }
        if (myBytes[2] == 0x01) {
//          digitalWrite(MotorTwoPin2, HIGH);
//          digitalWrite(MotorTwoPin1, LOW);
          rm.writeMicroseconds(2000);
//          delay(200);
        }
        if (myBytes[2] == 0x02) {
//          digitalWrite(MotorTwoPin2, LOW);
//          digitalWrite(MotorTwoPin1, HIGH);
          rm.writeMicroseconds(1000);
//          delay(200);
        }
        if (myBytes[3] != 0x00) {
//          analogWrite(MotorTwoSped, myBytes[3]);
        }
      }
      if(currentMillis - previousMillis> interval) {
          previousMillis = currentMillis;
          for (int x = 0; x < 4; x++) {
            myBytes[x] = 0x00;
          }
      }
   }
}
