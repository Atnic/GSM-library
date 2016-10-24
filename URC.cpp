#include "URC.h"

URC Urc;

URC::URC() {

}

bool URC::unsolicitedResultCode(const char urc[]) {
  const __FlashStringHelper *urcCallReady = F("Call Ready");
  const __FlashStringHelper *urcHttpAction = F("+HTTPACTION:");
  const __FlashStringHelper *urcEnterPin = F("+CPIN: ");
  const __FlashStringHelper *urcNewMessageIndication = F("+CMTI: ");
  const __FlashStringHelper *urcGetLocalTimestamp = F("*PSUTTZ: ");

  char *pointer;
  if((pointer = strstr_P(urc, (const char *)urcCallReady)) != NULL) {
    callReady.updated = true;
    return true;
  }
  if((pointer = strstr_P(urc, (const char *)urcHttpAction)) != NULL) {
    pointer += strlen_P((const char *)urcHttpAction);
		char *str = strtok(pointer, ",");
		unsigned char i = 0;
		for (i = 0; i < 3 && str != NULL; i++) {
			if(i == 0) httpAction.method = str[0] - '0';
			if(i == 1) httpAction.statusCode = atoi(str);
			if(i == 2) httpAction.dataLength = atoi(str);
			str = strtok(NULL, ",");
		}
    if(i >= 3) httpAction.updated = true;
    return true;
  }
  if((pointer = strstr_P(urc, (const char *)urcEnterPin)) != NULL) {
    pointer += strlen_P((const char *)urcEnterPin);
		char *str = strtok(pointer, "\"");
		strcpy(enterPin.code, str);
    enterPin.updated = true;
    return true;
  }
  if((pointer = strstr_P(urc, (const char *)urcGetLocalTimestamp)) != NULL) {
    pointer += strlen_P((const char *)urcEnterPin);
    char *str = strtok(pointer, ",\" +");
    for (unsigned char i = 0; i < 8 && str != NULL; i++) {
      if(i == 0) psuttz.year = atoi(str);
      if(i == 1) psuttz.month = atoi(str);
      if(i == 2) psuttz.day = atoi(str);
      if(i == 3) psuttz.hour = atoi(str);
      if(i == 4) psuttz.minute = atoi(str);
      if(i == 5) psuttz.second = atoi(str);
      if(i == 6) psuttz.timezone = atoi(str)/4;
			str = strtok(NULL, ",\" +");
    }
    psuttz.updated = true;
    return true;
  }
  if((pointer = strstr_P(urc, (const char *)urcNewMessageIndication)) != NULL) {
    pointer += strlen_P((const char *)urcNewMessageIndication);
		char *str = strtok(pointer, "\",");
		strcpy(newMessageIndication.mem, str);
    str = strtok(NULL, "\",");
    newMessageIndication.index = atoi(str);
    newMessageIndication.updated = true;
    return true;
  }
  return false;
}

void URC::resetUnsolicitedResultCode(void) {
  callReady.updated = false;
  enterPin.updated = false;
  httpAction.updated = false;
}
