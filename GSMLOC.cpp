#include "GSMLOC.h"

/* GSMLOC Class */
GSMLOC::GSMLOC(DTE &dte, IP &ip) {
  this->dte = &dte;
  this->ip = &ip;
  locationTime = (struct LocationTime){0, "", "", "", ""};
}

bool GSMLOC::atGSMLocationAndTime(unsigned char type, unsigned char cid) {
  const __FlashStringHelper *command = F("AT+CIPGSMLOC=%d,%d\r");
  const __FlashStringHelper *response = F("+CIPGSMLOC: ");
  char buffer[18];  // "AT+CIPGSMLOC=X,X\r"
  struct LocationTime locationTime;

  sprintf_P(buffer, (const char *)command, type, cid);

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(buffer)) return false;
  if (!dte->ATResponseContain(response, 60000)) return false;
  char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
  char *str = strtok(pointer, ",");
  for (size_t i = 0; i < 5 && str != NULL; i++) {
    if (i == 0) {
      locationTime.locationCode = (unsigned int)atol(str);
      if (locationTime.locationCode != 0) break;
    }
    if (type == 1 && i < 3) {
      if (i == 1) strcpy(locationTime.longitude, str);
      if (i == 2) strcpy(locationTime.latitude, str);
    }
    if (type == 2 || (type == 1 && 3 <= i && i <= 4)) {
      if (i == 1 || i == 3) strcpy(locationTime.date, str);
      if (i == 2 || i == 4) strcpy(locationTime.time, str);
    }
    str = strtok(NULL, ",");
  }
  if (!dte->ATResponseOk()) return false;
  this->locationTime = locationTime;
  return true;
}

struct LocationTime GSMLOC::getLocationTime(unsigned char cid) {
  if (strlen(locationTime.longitude) > 0) return locationTime;
  if (ip->getConnectionStatus(cid).status == 1) {
    if (!atGSMLocationAndTime(1, cid)) return (struct LocationTime){0, "", "", "", ""};
    if (locationTime.locationCode != 0) {
      if (!atGSMLocationAndTime(1, cid)) return (struct LocationTime){0, "", "", "", ""};
    }
  }
  return locationTime;
}
