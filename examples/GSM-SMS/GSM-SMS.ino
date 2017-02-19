#include <SoftwareSerial.h>

#include <DTE.h>
#include <GSM.h>
#include <SMS.h>
#include <URC.h>

#define PINPOWER 9
#define DEBUG false

SoftwareSerial SSerial(10, 11);
DTE dte(SSerial, PINPOWER, DEBUG);
SMS sms(dte);
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

  Serial.print("Try send SMS To ");
  Serial.println(gsm.getSubscriberNumber().number);
  Serial.println();
}

void loop(void) {
  if (!dte.AT())
    dte.powerReset();

  if (Urc.newMessageIndication.updated) {
    struct Message m = sms.readSMS(Urc.newMessageIndication.index);
    Serial.println(m.address);
    Serial.println(m.data);
    Urc.newMessageIndication.updated = false;
  }
}
