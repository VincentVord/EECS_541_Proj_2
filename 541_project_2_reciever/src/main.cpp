#include <Arduino.h>

const int BAUD_RATE = 2200;
const int rxPin = 2;
const unsigned long bitTimeUs = 1000000/BAUD_RATE;

// put function declarations here:
byte receiveData(bool &parityError, bool &framingError);

void setup() {
  Serial.begin(9600);
  pinMode(rxPin, INPUT);
}

char expected_message[12] = "Hello World";
int i = 0;

void loop() {

  bool parityError = false;
  bool framingError = false;

  byte recieved = receiveData(parityError, framingError);

  Serial.print("Recieved: ");
  Serial.println((char)recieved);

  if (recieved != expected_message[i]) {
    Serial.println("Data error!");
  }

  i++;

  if (i>= strlen(expected_message)) { //loop index back to 0 after reaching the end
    i = 0;
  }

  if (parityError) {
    Serial.println("Parity error!");
  }

  if (framingError) {
    Serial.println("Framing error!");
  }
}

// void loop() {
//     Serial.println(digitalRead(rxPin));
//     delay(100);
// }

byte receiveData (bool &parityError, bool &framingError) {

  while (digitalRead(rxPin) == LOW) {
    //idle
  }

  while (digitalRead(rxPin) == HIGH) {
    //wait for high->low transition
  }

  // input is no longer idle: sees the low start bit
  delayMicroseconds(bitTimeUs/2); //move to the center of the start bit
                                  // from here on out, we wil always be measuring each bit 
                                  // at the middle of its timestamp

  if (digitalRead(rxPin) != LOW) { //make sure we are still in the LOW start bit
    framingError = true;
    return 0;
  }

  delayMicroseconds(bitTimeUs); //move to the center of the first data bit

  byte data = 0;
  int oneBitTotal = 0;

  for (int i=0; i<8; i++) { //read the 8 data bits
    int bit = digitalRead(rxPin);
    if (bit == HIGH) {
      data |= (1 << i);
      oneBitTotal++;
    }
    delayMicroseconds(bitTimeUs); //measure at the baud rate
  }

  int parityBit = digitalRead(rxPin); //read parity bit

  int expectedParityBit = oneBitTotal % 2;

  if (parityBit != expectedParityBit) { //check parity bit
    parityError = true;
  }

  delayMicroseconds(bitTimeUs); //move to the middle of the stop bit
  
  int stopBit = digitalRead(rxPin); // read stop bit
  if (stopBit != HIGH) {
    framingError = true;
  }

  return data;

}

