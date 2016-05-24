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

  bool atAttachGPRSService(void);
  bool atAttachGPRSService(bool attach);

public:
  GPRS(DTE &dte);

  bool isAttached(void);
};

#endif
