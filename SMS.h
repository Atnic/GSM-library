/*
*
*/
#ifndef SMS_h
#define SMS_h

#include <Arduino.h>
#include <DTE.h>

class SMS
{
private:
  DTE *dte;
  struct Message {
    unsigned char index = 0;
    unsigned char status = 255;
    char address[17];
    char timestamp[21];
    unsigned char firstOctet = 0;
    unsigned int mr = 0;
    char data[163];
  } message[2];

  unsigned char messageStatusIndex(const char status[]);

public:
  SMS(DTE &dte);

  bool atDeleteSMS(unsigned char index, unsigned char delFlag = 0);

  bool atSelectSMSFormat(void);
  bool atSelectSMSFormat(unsigned char mode);

  bool atListSMS(unsigned char status = 0, unsigned char mode = 0);

  bool atReadSMS(unsigned char index, unsigned char mode = 0);

  bool atSendSMS(const char destination[], const char message[]);

  void init(unsigned char smsFormat = 1);

  bool deleteSMS(unsigned char index) { return atDeleteSMS(index); };
  bool deleteSMSAllRead(void) { return atDeleteSMS(0, 1); };
  bool deleteSMSAllReadSent(void) { return atDeleteSMS(0, 2); };
  bool deleteSMSAllReadSentUnsent(void) { return atDeleteSMS(0, 3); };
  bool deleteSMSAll(void) { return atDeleteSMS(0, 4); };

  void sendSMS(const char destination[], const char message[]);
};

#endif
