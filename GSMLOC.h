#ifndef GSMLOC_h
#define GSMLOC_h

#include <Arduino.h>
#include "DTE.h"
#include "IP.h"

struct LocationTime {
  unsigned int locationCode;
  char longitude[12];
  char latitude[12];
  char date[11];
  char time[9];
};

class GSMLOC {
 private:
  DTE *dte;
  IP *ip;
  struct LocationTime locationTime;

 public:
  GSMLOC(DTE &dte, IP &ip);

  /**
   * Command AT+GSMLOC=
   * @param  type Type of command
   *              1: View location, latitude, and time
   *              2: View time
   * @param  cid  Context Identifier, See IP Class, default 1
   * @return      true: If successful, false: Otherwise
   */
  bool atGSMLocationAndTime(unsigned char type, unsigned char cid = 1);

  /**
   * getLocationTime
   * @return  LocationTime Struct
   */
  struct LocationTime getLocationTime(unsigned char cid = 1);
};

#endif
