#include <SoftwareSerial.h>

#include <DTE.h>
#include <GSM.h>
#include <URC.h>

#define PINPOWER 9
#define DEBUG false

SoftwareSerial SSerial(10, 11);
DTE dte(SSerial, PINPOWER, DEBUG);
GSM gsm(dte);

char buffer[5], a;
int x = 0, p;

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
    a = Serial.read();
    if ((a != '\n') && (x < 6)) {
      buffer[x] = a;
      Serial.print(buffer[x]);
      x++;
    }
    if (a == '\n') {
      buffer[x - 1] = '\0';
      p = strlen(buffer);
      char temp[p];
      strcpy(temp, buffer);

      Serial.println();
      // Serial.print(p);
      gsm.sendServiceData(temp);
      x = 0;
      // while(x<6) {buffer[x]='null'; x++;}
    }
  }

  if (Urc.serviceDataIndication.updated) {
    Serial.println(Urc.serviceDataIndication.str);
    Urc.serviceDataIndication.updated = false;
  }
}
