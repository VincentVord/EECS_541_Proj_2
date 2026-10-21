#include <Arduino.h>

const int BAUD_RATE = 2200;
const int rxPin = 2;
const unsigned long bitTimeUs = 1000000/BAUD_RATE;

unsigned long totalBits = 0;
unsigned long errorBits = 0;
unsigned long totalBytes = 0;
unsigned long errorBytes = 0;

// put function declarations here:
byte receiveData(bool &parityError, bool &framingError);

void setup() {
  Serial.begin(9600);
  pinMode(rxPin, INPUT);
}

// expecting "H"
char expected_message[2] = "H";
int i = 0;

void loop() {

  bool parityError = false;
  bool framingError = false;

  byte received = receiveData(parityError, framingError);

  Serial.print("Recieved: ");
  Serial.println((char)received);

  byte expected = expected_message[i];

  totalBytes++;
  totalBits += 8;

  byte difference = received ^ expected;

  int bitErrors = 0;

  for (int bit = 0; bit < 8; bit++) {
    if ((difference >> bit) & 0x01) {
      bitErrors++;
    }
  }

  errorBits += bitErrors;

  if (received != expected) {
    errorBytes++;
  }

  i++;

  if (i >= strlen(expected_message)) {
    i = 0;
  }

  if (totalBytes % 100 == 0) {

    float bitErrorRate =
        100.0 * errorBits / totalBits;

    float byteErrorRate =
        100.0 * errorBytes / totalBytes;

    Serial.print("BER: ");
    Serial.print(bitErrorRate);
    Serial.println("%");

    Serial.print("Byte error rate: ");
    Serial.print(byteErrorRate);
    Serial.println("%");
  }
}

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
    // return 0;
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

