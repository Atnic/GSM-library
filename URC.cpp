#include "URC.h"

URC urc;

URC::URC() {

}

bool URC::unsolicitedResultCode(const char urc[]) {
  Serial.print("URC: ");
  Serial.print(urc);

  char *pointer;
  if((pointer = strstr_P(urc, (const char *)F("+HTTPACTION:"))) != NULL) {
    pointer += strlen_P((const char *)F("+HTTPACTION:"));
		char *str = strtok(pointer, ",\r\n");
		unsigned char i = 0;
		for (i = 0; i < 3 && str != NULL; i++) {
			if(i == 0) HTTPAction.method = str[0] - '0';
			if(i == 1) HTTPAction.statusCode = atoi(str);
			if(i == 2) HTTPAction.dataLength = atoi(str);
			str = strtok(NULL, ",\r\n");
		}
    HTTPAction.updated = true;
    return true;
  }
  return false;
}
