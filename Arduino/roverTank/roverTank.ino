#include <SPI.h>
#include <Servo.h>
#include "Adafruit_BLE_UART.h"

#define ADAFRUITBLE_REQ 10
#define ADAFRUITBLE_RDY 2
#define ADAFRUITBLE_RST 9

#define RIGHTMOTOR 4
#define LEFTMOTOR 7

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
    int myBytes[2];
    if(currentMillis - previousMillis> interval) {
        previousMillis = currentMillis;
        for (int x = 0; x < 2; x++) {
          myBytes[x] = 0x00;
        }
    }
    if (BTLEserial.available()) {
        for (int x = 0; x < 2; x++) {
          myBytes[x] = BTLEserial.read();
          previousMillis = currentMillis;
        }
          lm.writeMicroseconds((myBytes[0] * 10));
          rm.writeMicroseconds((myBytes[1] * 10));
      }
   }
}
