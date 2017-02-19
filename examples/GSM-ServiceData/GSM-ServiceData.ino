#include <SoftwareSerial.h>

#include <DTE.h>
#include <GSM.h>
#include <URC.h>

#define PINPOWER 9
#define DEBUG false

SoftwareSerial SSerial(10, 11);
DTE dte(SSerial, PINPOWER, DEBUG);
GSM gsm(dte);

void setup(void) {
  Serial.begin(9600);
  SSerial.begin(19200);
  Serial.println("Turning ON SIM900...");
  dte.powerReset();
  Serial.println("Waiting Network...");
  while (gsm.getNetworkRegistration().status != 1)
    ;
  Serial.print("Connecting ");
  Serial.println(gsm.getOperator().oper);

  Serial.print("input service operator number (ex : *123#)");
  Serial.println();
}

void loop(void) {
  if (!dte.AT()) {
    dte.powerReset();
  }

  if (Serial.available() > 0) {
    char buffer[20];
    unsigned char x = Serial.readBytes(buffer, 20);
    buffer[x] = '\0';
    gsm.sendServiceData(buffer);
  }

  if (Urc.serviceDataIndication.updated) {
    Serial.println(Urc.serviceDataIndication.str);
    Urc.serviceDataIndication.updated = false;
  }
}
