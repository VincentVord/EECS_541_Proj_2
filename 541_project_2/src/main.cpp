#include <Arduino.h>

const int BAUD_RATE = 2200;
const int txPin = 13;
const unsigned long delayMs = 1000/BAUD_RATE;

// put function declarations here:
void transmitData(byte);

void setup() {
  // start serial connection
  Serial.begin(9600);
  
  pinMode(txPin, OUTPUT); //set the tx pin 
  digitalWrite(txPin, HIGH); //set the tx pin default
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

void transmitData(byte data) { //transmits one 11-bit packet
  // start bit
  digitalWrite(txPin, LOW);
  delay(delayMs);

  int oneBitTotal = 0;

  for (int i = 0; i < 8; i++) {
    int bit = (data >> i) & 0x01;
    digitalWrite(txPin, bit);

    Serial.print("Transmitted bit ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(bit);

    if (bit==1) {
      oneBitTotal +=1;
    }

    delay(delayMs);
  }

  // even parity bit
  if (oneBitTotal%2==1) { //odd number of 1s
    digitalWrite(txPin, HIGH); //make the parity bit 1
  } else { //even number of 1s
    digitalWrite(txPin, LOW); //make the parity bit 0
  }
  delay(delayMs);

  // stop bit
  digitalWrite(txPin, HIGH);
  delay(delayMs);
}