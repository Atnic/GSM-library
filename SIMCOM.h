#ifndef SIMCOM_h
#define SIMCOM_h

#include <Arduino.h>
#include "DTE.h"

class SIMCOM {
 private:
  DTE *dte;

  bool allowGetTimestamp = false;
  bool atSetModeGetTimestamp(bool mode);

 public:
  SIMCOM(DTE &dte);

  bool isAllowGetTimestamp(void) { return this->allowGetTimestamp;};
}

#endif
