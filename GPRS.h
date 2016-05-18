/*
*
*/
#ifndef GPRS_h
#define GPRS_h

#include <Arduino.h>
#include <DTE.h>

class GPRS
{
private:
  DTE *dte;
  bool attached = false;
  struct PDPContext {
    unsigned char contextIdentifier;
    char type[5];
    char apn[15];
    char address[18];
  } pdpContext[3];
  struct NetworkRegistration {
    unsigned char mode;
    unsigned char status;
    char locationAreaCode[3];
    char cellID[3];
  } networkRegistration;

  void initializeVariable(void);

public:
  GPRS(DTE &dte);

  bool atAttachGPRSService(void);
  bool atAttachGPRSService(bool attach);

  bool atDefinePDPContext(void);
  bool atDefinePDPContext(unsigned char cid, const char *type = NULL, const char *apn = "", const char *address = "0.0.0.0");

  bool atNetworkRegistrationStatus(void);
  bool atNetworkRegistrationStatus(unsigned char n);

  bool isAttached(void) { return attached; }
  unsigned char getNetworkRegistrationStatus(void) { return networkRegistration.status; }
};

#endif
