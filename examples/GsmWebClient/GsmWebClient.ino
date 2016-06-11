#include <SoftwareSerial.h>
#include <URC.h>
#include <DTE.h>
#include <GSM.h>
#include <GPRS.h>
#include <IP.h>
#include <HTTP.h>

#define PINPOWER  9
#define DEBUG     false
#define APN       "internet"
#define USERNAME  ""
#define PASSWORD  ""

SoftwareSerial SSerial(10, 11);

DTE dte(SSerial, PINPOWER, DEBUG);
GSM gsm(dte);
GPRS gprs(dte);
IP ip(dte, gprs);
HTTP http(dte, ip);

void setup() {
  Serial.begin(9600);
  SSerial.begin(19200); // The fastest and yet safe speed for SoftwareSerial

  Serial.println("Power Reset\r\n");
  dte.powerReset();
  ip.setConnectionParamGprs(APN, USERNAME, PASSWORD);
}

void loop() {
  if(!dte.AT())
    dte.powerReset();

  Serial.print("Network Registration: ");
  Serial.println(gsm.getNetworkRegistration().status);
  Serial.print("GPRS Attached: ");
  Serial.println(gprs.isAttached());
  Serial.println("Wait to connect...");
  Serial.println();
  if(ip.openConnection()) { // Open Connection, if it's already connected it just return true
    Serial.println("Connected!");
    unsigned long timeout = 30; // The minimum HTTP Timeout setting
    http.initialize(timeout);
    http.action("GET", "arduino.cc/asciilogo.txt");
    unsigned long t = millis();
    while(millis() - t < timeout*1000 && !Urc.httpAction.updated)
      dte.clearReceivedBuffer(); // This is necessary, so URC can be captured
    if(Urc.httpAction.updated && Urc.httpAction.statusCode == 200) {
      Serial.println("Data Received: ");

      for(unsigned long i = 0; i < Urc.httpAction.dataLength;) {
        char response[101]; // Plus 1 for terminate null char
        http.readDataReceived(response, sizeof(response)-1, i);
        i += sizeof(response)-1;
        Serial.print(response);
      }
      Serial.println();

      /**
       * Code below can also be used, but it take so much memory.
       * Depend on Urc.httpAction.dataLength, but it is not necessary to read
       * all Server Response.
       */
      //char response[Urc.httpAction.dataLength + 1];
      //http.readDataReceived(response, Urc.httpAction.dataLength);
      //Serial.print(response);
      //Serial.println();
    }
    else {
      if (Urc.httpAction.updated) {
        Serial.print("Status Code: ");
        Serial.println(Urc.httpAction.statusCode);
      }
      Serial.println("Failed");
    }
    http.terminate();
    Serial.println("Done!");
    Serial.println("Press any key to see next connection behave...");
    Serial.println("*It's already connected, so it's faster. :)");
    while (Serial.available() == 0);
    while (Serial.available() > 0) {
      Serial.read();
      delay(50);
    }
  }
  delay(1000); //It's just for debugging, no need to delay actually
}
