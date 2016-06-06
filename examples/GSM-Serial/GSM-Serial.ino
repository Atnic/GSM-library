#include <SoftwareSerial.h>

SoftwareSerial SSerial(10, 11);

void setup(void) {
  Serial.begin(9600);
  SSerial.begin(9600);
}

void loop(void) {
  if (Serial.available() > 0) {
    SSerial.write(Serial.read());
  }
  if (SSerial.available() > 0) {
    Serial.write(SSerial.read());
  }
}

