#ifndef SIMCOM_h
#define SIMCOM_h

#include <Arduino.h>
#include "DTE.h"

class SIMCOM {
 private:
  DTE *dte;

  bool allowGetTimestamp = false;

 public:
  SIMCOM(DTE &dte);

  bool atSetModeGetTimestamp(bool mode = true);
  bool isAllowGetTimestamp(void) { return this->allowGetTimestamp;};
};

#endif
