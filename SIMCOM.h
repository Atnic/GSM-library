#ifndef SIMCOM_h
#define SIMCOM_h

#include <Arduino.h>
#include "DTE.h"

class SIMCOM {
 private:
  DTE *dte;

 public:
  SIMCOM(DTE &dte);

  /**
   * Command AT+CLTS=
   * @param  mode Control Get Timestamp mode
   *             false: Disable
   *             true: Enable
   * @return     true: If command successful, false: Otherwise
   */
  bool atSetModeGetTimestamp(bool mode);

  /**
   * Set Mode Get Timestamp
   */
  bool setModeGetTimestamp(bool mode = true);
};

#endif
