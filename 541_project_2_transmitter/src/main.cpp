#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>   

const int BAUD_RATE = 2200;
const int txPin = 13;
const unsigned long delayUs = 1000000/BAUD_RATE;

// put function declarations here:
void transmitData(byte);
int introduceError(int);

void setup() {
  // start serial connection
  Serial.begin(9600);
  
  pinMode(txPin, OUTPUT); //set the tx pin 
  digitalWrite(txPin, HIGH); //set the tx pin default

  randomSeed(analogRead(A0)); //seed the random generator
}

void loop() {

  char message[12] = "Hello World";

  for (int i = 0; i<12; i++) {
    byte data = message[i];

    Serial.print("Sending: ");
    Serial.println((char)data);

    transmitData(data);
    digitalWrite(txPin, HIGH);
  }
}

// DATA FORMAT //
// |------------------------|
// |1|       8          |1|1|
// |^        ^           ^ ^|
// ||        |       ____| ||
// ||        |      |      ||
// |start  data  parity stop|
// |bit    bits    bit   bit|
// |------------------------|

// use even parity bit

int introduceError(int bit) { // has a 50% chance to invert the bit
  if (random(2) == 1) {
    return !bit;
  }
  return bit;
}

void transmitData(byte data) { //transmits one 11-bit packet
  // start bit
  digitalWrite(txPin, LOW);
  delayMicroseconds(delayUs);

  int oneBitTotal = 0;

  for (int i = 0; i < 8; i++) {
    int bit = (data >> i) & 0x01;

    if (bit==1) { //count bits for parity
      oneBitTotal +=1;
    }

    int finalBit = introduceError(bit);
    digitalWrite(txPin, finalBit);

    Serial.print("Transmitted bit ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(finalBit);


    delayMicroseconds(delayUs);
  }

  // even parity bit
  if (oneBitTotal%2==1) { //odd number of 1s
    digitalWrite(txPin, HIGH); //make the parity bit 1
  } else { //even number of 1s
    digitalWrite(txPin, LOW); //make the parity bit 0
  }
  delayMicroseconds(delayUs);

  // stop bit
  digitalWrite(txPin, HIGH);
  delayMicroseconds(delayUs);
}