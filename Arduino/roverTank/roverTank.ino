/*********************************************************************
This is an example for our nRF8001 Bluetooth Low Energy Breakout

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/1697

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Kevin Townsend/KTOWN  for Adafruit Industries.
MIT license, check LICENSE for more information
All text above, and the splash screen below must be included in any redistribution
*********************************************************************/

// This version uses the internal data queing so you can treat it like Serial (kinda)!

#include <SPI.h>
#include "Adafruit_BLE_UART.h"

// Connect CLK/MISO/MOSI to hardware SPI
// e.g. On UNO & compatible: CLK = 13, MISO = 12, MOSI = 11
#define ADAFRUITBLE_REQ 10
#define ADAFRUITBLE_RDY 2     // This should be an interrupt pin, on Uno thats #2 or #3
#define ADAFRUITBLE_RST 9

#define MotorOnePin4 8
#define MotorOnePin3 7
#define MotorOneSped 6

#define MotorTwoPin2 4
#define MotorTwoPin1 3
#define MotorTwoSped 5

Adafruit_BLE_UART BTLEserial = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);
char gChar[4];
/**************************************************************************/
/*!
    Configure the Arduino and start advertising with the radio
*/
/**************************************************************************/
void setup(void)
{ 
  Serial.begin(9600);
  while(!Serial); // Leonardo/Micro should wait for serial init
  Serial.println(F("Adafruit Bluefruit Low Energy nRF8001 Print echo demo"));

  //BTLEserial.setDeviceName("TANK!!"); /* 7 characters max! */
  
  BTLEserial.begin();
  pinMode(MotorOnePin4, OUTPUT);
  pinMode(MotorOnePin3, OUTPUT);
  pinMode(MotorOneSped, OUTPUT);
  
  pinMode(MotorTwoPin2, OUTPUT);
  pinMode(MotorTwoPin1, OUTPUT);
  pinMode(MotorTwoSped, OUTPUT);
  
}

/**************************************************************************/
/*!
    Constantly checks for new events on the nRF8001
*/
/**************************************************************************/
aci_evt_opcode_t laststatus = ACI_EVT_DISCONNECTED;

void loop()
{
  // Tell the nRF8001 to do whatever it should be working on.
  BTLEserial.pollACI();

  // Ask what is our current status
  aci_evt_opcode_t status = BTLEserial.getState();
  // If the status changed....
  if (status != laststatus) {
    // print it out!
    if (status == ACI_EVT_DEVICE_STARTED) {
        Serial.println(F("* Advertising started"));
    }
    if (status == ACI_EVT_CONNECTED) {
        Serial.println(F("* Connected!"));
    }
    if (status == ACI_EVT_DISCONNECTED) {
        Serial.println(F("* Disconnected or advertising timed out"));
        digitalWrite(MotorTwoPin2, HIGH);
        digitalWrite(MotorTwoPin1, HIGH);
                                             digitalWrite(MotorOnePin4, HIGH);
        digitalWrite(MotorOnePin3, HIGH);
        
    }
    // OK set the last status change to this one
    laststatus = status;
  }

  if (status == ACI_EVT_CONNECTED) {
    // Lets see if there's any data for us!
    if (BTLEserial.available()) {
      Serial.print("* "); Serial.print(BTLEserial.available()); Serial.println(F(" bytes available from BTLE"));
    }
    // OK while we still have something to read, get a character and print it out
    byte myHex[4];
    int len = BTLEserial.available();
    int x = 0;
    while (BTLEserial.available()) {
        byte c = BTLEserial.read();
        myHex[x] = (c,HEX);
        x++;
        Serial.print("c: ");Serial.print(c,HEX);Serial.print("   ");
        Serial.print("myChar: ");Serial.println(myHex[x],HEX);
    }
    
    if (myHex[0] == 0x00) {
        digitalWrite(MotorOnePin4, LOW);
        digitalWrite(MotorOnePin3, LOW);
    }
    if (myHex[0] == 0x01) {
        digitalWrite(MotorOnePin4, HIGH);
        digitalWrite(MotorOnePin3, LOW);
    }
    if (myHex[0] == 0x02) {
        digitalWrite(MotorOnePin4, LOW);
        digitalWrite(MotorOnePin3, HIGH);
    }
    if (myHex[1] != 0x00) {
        //digitalWrite(MotorTwoSped, HIGH);
        analogWrite(MotorOneSped, (myHex[1]));
    }
    
    
    
    if (myHex[0] == 0x00) {
        digitalWrite(MotorTwoPin2, LOW);
        digitalWrite(MotorTwoPin1, LOW);
    }
    if (myHex[2] == 0x01) {
        digitalWrite(MotorTwoPin2, HIGH);
        digitalWrite(MotorTwoPin1, LOW);
    }
    if (myHex[2] == 0x02) {
        digitalWrite(MotorTwoPin2, LOW);
        digitalWrite(MotorTwoPin1, HIGH);
    }
    if (myHex[3] != 0x00) {
        //digitalWrite(MotorTwoSped, HIGH);
        analogWrite(MotorTwoSped, (myHex[3]));
    }
  }
}


