#include <SPI.h>
#include <Servo.h>
#include "Adafruit_BLE_UART.h"

#define ADAFRUITBLE_REQ 10
#define ADAFRUITBLE_RDY 2
#define ADAFRUITBLE_RST 9

#define RIGHTMOTOR 7
#define LEFTMOTOR 4

Servo rm;
Servo lm;

Adafruit_BLE_UART BTLEserial = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);

void setup(void) {
  Serial.begin(9600);
  while(!Serial);
  Serial.println(F("Adafruit Bluefruit Low Energy nRF8001 Print echo demo"));
  
  BTLEserial.begin();
  
  rm.attach(RIGHTMOTOR);
  lm.attach(LEFTMOTOR);

}

aci_evt_opcode_t laststatus = ACI_EVT_DISCONNECTED;

void loop() {
  unsigned long currentMillis = millis();
  unsigned long previousMillis;
  unsigned long interval = 3000;
  BTLEserial.pollACI();
  aci_evt_opcode_t status = BTLEserial.getState();
  if (status != laststatus) {
    if (status == ACI_EVT_CONNECTED) {
      previousMillis = currentMillis;
    }
    if (status == ACI_EVT_DISCONNECTED) {
      lm.writeMicroseconds(1500);
      rm.writeMicroseconds(1500);
    }
    laststatus = status;
  }

  if (status == ACI_EVT_CONNECTED) {
    int myBytes[4];
    if(currentMillis - previousMillis> interval) {
        previousMillis = currentMillis;
        for (int x = 0; x < 4; x++) {
          myBytes[x] = 0x00;
        }
    }
    if (BTLEserial.available()) {
        for (int x = 0; x < 4; x++) {
          myBytes[x] = BTLEserial.read();
          previousMillis = currentMillis;
        }
        if (myBytes[0] == 0x00) {
          lm.writeMicroseconds(1500);
        }
        if (myBytes[0] == 0x01) {
          lm.writeMicroseconds(1500);
//          delay(20);
          lm.writeMicroseconds(2000);
        }
        if (myBytes[0] == 0x02) {
          lm.writeMicroseconds(1500);
//          delay(20);
          lm.writeMicroseconds(1000);
        }
        if (myBytes[2]  == 0x00) {
          rm.writeMicroseconds(1500);
        }
        if (myBytes[2] == 0x01) {
          rm.writeMicroseconds(1500);
//          delay(20);
          rm.writeMicroseconds(2000);
        }
        if (myBytes[2] == 0x02) {
          rm.writeMicroseconds(1500);
//          delay(20);
          rm.writeMicroseconds(1000);
        }
      }
   }
}
