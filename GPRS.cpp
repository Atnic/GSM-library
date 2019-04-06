#include "GPRS.h"
#include "URC.h"

/* GPRS Class */
GPRS::GPRS(DTE &dte) {
  this->dte = &dte;
}

bool GPRS::atAttachGPRSService(void) {
  const __FlashStringHelper *command = F("AT+CGATT?\r");
  const __FlashStringHelper *response = F("+CGATT: ");

  attached = false;
  dte->clearReceivedBuffer();
  if (!dte->ATCommand(command)) return false;
  if (!dte->ATResponseContain(response)) return false;
  char *str = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
  if (!dte->ATResponseOk()) return false;
  attached = (str[0] == '1') ? true : false;
  return true;
}

bool GPRS::atAttachGPRSService(bool attach) {
  const __FlashStringHelper *command = F("AT+CGATT=%d\r");
  char buffer[12];  // "AT+CGATT=X\r"

  sprintf_P(buffer, (const char *)command, (attach) ? 1 : 0);

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(buffer)) return false;
  if (!dte->ATResponseOk(10000)) return false;
  this->attached = attach;
  return true;
}

bool GPRS::isAttached(void) {
  if (!atAttachGPRSService()) attached = false;
  return attached;
}
