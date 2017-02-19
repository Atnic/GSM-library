#include <SoftwareSerial.h>

#include <DTE.h>

#define PINPOWER 9
#define DEBUG true

SoftwareSerial SSerial(10, 11);
DTE dte(SSerial, PINPOWER, DEBUG);

void setup(void) {
  Serial.begin(9600);
  SSerial.begin(19200);

  dte.powerReset();
  dte.setFlowControl(0, 0);  //Turn off FlowControl, because defaults on.
}

void loop(void) {
  if (Serial.available() > 0) {
    SSerial.write(Serial.read());
  }
  while (SSerial.available() > 0) {
    Serial.write(SSerial.read());
  }
}
