/*
*
*/
#ifndef SIMCOM_h
#define SIMCOM_h

#include <Arduino.h>
#include <DTE.h>

class SIMCOM
{
private:
  DTE *dte;
  bool indicateRI = false;
  bool localTimestampEnabled = false;
  bool simInsertedURCEnabled = false;
  bool simInserted = false;
  unsigned char slowClockMode = 0;
  unsigned char ussdResultCodePresentation = 0;
  unsigned char ussdStatus = 0;

public:
  SIMCOM(DTE &dte);

  bool atPowerOff(bool normal = true);

  bool atIndicateRIWhenUsingURC(void);
  bool atIndicateRIWhenUsingURC(bool indicate);

  bool atLocalTimestap(bool enable);

  bool atSimInsertedStatusReporting(void);
  bool atSimInsertedStatusReporting(bool urc);

  bool atConfigureSlowClock(void);
  bool atConfigureSlowClock(unsigned char mode);

  bool atDeleteAllSMS(unsigned char type = 6);
  bool atDeleteAllSMSRead(void) { return atDeleteAllSMS(1); };
  bool atDeleteAllSMSUnread(void) { return atDeleteAllSMS(2); };
  bool atDeleteAllSMSSent(void) { return atDeleteAllSMS(3); };
  bool atDeleteAllSMSUnsent(void) { return atDeleteAllSMS(4); };
  bool atDeleteAllSMSInbox(void) { return atDeleteAllSMS(5); };

  bool atUnstructuredSupplementaryServiceData(void);
  bool atUnstructuredSupplementaryServiceData(unsigned char n, const char str[] = "", unsigned char dcs = 0);
  bool atUnstructuredSupplementaryServiceData(unsigned char n, const __FlashStringHelper str[], unsigned char dcs = 0);

  bool isIndicateRI(void) { return indicateRI; }
  bool isLocalTimestampEnabled(void) { return localTimestampEnabled; }
  bool isSimInsertedURCEnabled(void) { return simInsertedURCEnabled; }
  bool isSimInserted(void) { return simInserted; }
  unsigned char getSlowClockMode(void) { return slowClockMode; }
};

#endif
