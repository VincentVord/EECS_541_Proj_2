#include <Arduino.h>

const int BAUD_RATE = 1;
const int txPin = 13;
const unsigned long delayUs = 1000000/BAUD_RATE;

// put function declarations here:
void transmitData(byte);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(txPin, OUTPUT);
  digitalWrite(txPin, HIGH);
}

void loop() {
  // byte dataBuf[10];
  // if (Serial.available() > 0) {
  //   size_t bytesRead = Serial.readBytes(dataBuf, 10);

  //   Serial.print("Successfully read ");
  //   Serial.print(bytesRead);
  //   Serial.println(" bytes.");

  //   for (size_t i = 0; i < bytesRead; i++) {
  //     transmitData(dataBuf[i]);
  //   }
  // }

  if (Serial.available() > 0) {

    byte data = Serial.read();

    Serial.print("Received: ");
    Serial.println((char)data);

    transmitData(data);
    digitalWrite(txPin, HIGH);
  }
}

// put function definitions here:
void transmitData(byte data) {
  for (int i = 0; i < 8; i++) {
    int bit = (data >> i) & 0x01;
    digitalWrite(txPin, bit);

    Serial.print("Transmitted bit ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(bit);

    delayMicroseconds(delayUs);
  }
}