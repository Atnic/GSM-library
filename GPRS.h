#ifndef GPRS_h
#define GPRS_h

#include <Arduino.h>
#include "DTE.h"

class GPRS {
 private:
  DTE *dte;
  bool attached = false;

 public:
  GPRS(DTE &dte);

  /**
   * Command AT+CGATT?
   * @return  true: If command success, false: Otherwise or timeout
   */
  bool atAttachGPRSService(void);

  /**
   * Command AT+CGATT=
   * @param attach true: Force attach, false: detach GPRS Service
   * @return       true: If command success, false: Otherwise or timeout
   */
  bool atAttachGPRSService(bool attach);

  /**
   * Is GPRS Service attached/available
   * @return  true: If attached/available, false: Otherwise
   */
  bool isAttached(void);
};

#endif
