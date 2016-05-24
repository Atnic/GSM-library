#include "URC.h"

URC urc;

URC::URC() {

}

bool URC::unsolicitedResultCode(const char urc[]) {
  const __FlashStringHelper *urcCallReady = F("Call Ready");
  const __FlashStringHelper *urcHttpAction = F("+HTTPACTION:");
  const __FlashStringHelper *urcEnterPin = F("+CPIN: ");

  Serial.print(F("URC: "));
  Serial.println(urc);

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
    httpAction.updated = true;
    return true;
  }
  if((pointer = strstr_P(urc, (const char *)urcEnterPin)) != NULL) {
    pointer += strlen_P((const char *)urcEnterPin);
		char *str = strtok(pointer, "\"");
		strcpy(enterPin.code, str);
    enterPin.updated = true;
    return true;
  }
  return false;
}

void URC::resetUnsolicitedResultCode(void) {
  callReady.updated = false;
  enterPin.updated = false;
  httpAction.updated = false;
}
