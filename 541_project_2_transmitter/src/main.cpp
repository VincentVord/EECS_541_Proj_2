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

  char message[2] = "H";

  for (int i = 0; i<strlen(message); i++) {
    byte data = message[i];

    Serial.print("Sending: ");
    Serial.println((char)data);

    transmitData(data);
    digitalWrite(txPin, HIGH);
    delay(100); //keep data idle (HIGH) for 100ms
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

int introduceError(int bit) { // has a 5% chance to invert the bit
  if (random(20) == 1) {
    return !bit;
  }
  return bit;
}

void transmitData(byte data) {

  byte finalData = 0;

  // Generate corrupted byte BEFORE transmission starts
  for (int i = 0; i < 8; i++) {

    int bit = (data >> i) & 0x01;

    int finalBit = introduceError(bit);

    if (finalBit == 1) {
      finalData |= (1 << i);
    }
  }

  // Calculate parity from ORIGINAL data
  int oneBitTotal = 0;

  for (int i = 0; i < 8; i++) {
    if ((data >> i) & 0x01) {
      oneBitTotal++;
    }
  }

  // Start bit
  digitalWrite(txPin, LOW);
  delayMicroseconds(delayUs);

  // Data bits
  for (int i = 0; i < 8; i++) {

    int bit = (finalData >> i) & 0x01;

    digitalWrite(txPin, bit);
    delayMicroseconds(delayUs);
  }

  // Even parity bit
  if (oneBitTotal % 2 == 1) {
    digitalWrite(txPin, HIGH);
  } else {
    digitalWrite(txPin, LOW);
  }

  delayMicroseconds(delayUs);

  // Stop bit
  digitalWrite(txPin, HIGH);
  delayMicroseconds(delayUs);
}