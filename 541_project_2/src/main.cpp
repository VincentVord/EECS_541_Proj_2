#include <Arduino.h>

const int BAUD_RATE = 2200;

// put function declarations here:
int transmit(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = transmit(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int transmit(int x, int y) {
  return x + y;
}