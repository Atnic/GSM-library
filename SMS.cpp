#include "SMS.h"
#include "URC.h"

unsigned char SMS::messageStatusIndex(const char *status) {
  if(strcmp_P(status, (const char *)F("REC UNREAD")) == 0) return 0;
  if(strcmp_P(status, (const char *)F("REC READ")) == 0) return 1;
  if(strcmp_P(status, (const char *)F("STO UNSENT")) == 0) return 2;
  if(strcmp_P(status, (const char *)F("STO SENT")) == 0) return 3;
  return 255;
}

/* SMS Class */
SMS::SMS(DTE &dte)
{
	this->dte = &dte;
}

bool SMS::atDeleteSMS(unsigned char index, unsigned char delFlag) {
	char buffer[strlen_P((const char *)F("AT+CMGD=X,X\r\r\n")) + 1];

	sprintf(buffer, "AT+CMGD=%d,%d\r", index, delFlag);

	dte->clearReceivedBuffer();
	dte->ATCommand(buffer);
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(strcat(buffer, "\r\n"))) return false;
	}
	if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
	return true;
}

bool SMS::atSelectSMSFormat(void) {
	unsigned char format;

	dte->clearReceivedBuffer();
  dte->ATCommand(F("AT+CMGF?\r"));
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(F("AT+CMGF?\r\r\n"))) return false;
	}
	if(!dte->ATResponseContain(F("+CMGF: "))) return false;
	char *pointer = strstr(dte->getResponse(), "+CMGF: ") + strlen_P((const char *)F("+CMGF: "));
	char *str = strtok(pointer, "\r\n");
	format = str[0] - '0';
	if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
  dte->setPDU((format == 0)?true:false);
	return true;
}

bool SMS::atSelectSMSFormat(unsigned char mode) {
	char buffer[strlen_P((const char *)F("AT+CMGF=X\r\r\n")) + 1];

	sprintf(buffer, "AT+CMGF=%d\r", mode);

	dte->clearReceivedBuffer();
	dte->ATCommand(buffer);
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(strcat(buffer, "\r\n"))) return false;
	}
	if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
  dte->setPDU((mode == 0)?true:false);
	return true;
}

bool SMS::atListSMS(unsigned char status, unsigned char mode) {
	char buffer[strlen_P((const char *)F("AT+CMGL=\"XXXXXXXXXX\",X\r\r\n")) + 1];
  char command[] = "AT+CMGL=";
  struct Message message;

  if(dte->isPDU())
    sprintf(buffer, "%s%d,%d\r", command, status, mode);
  else {
    char statusText[11];
    switch (status) {
      case 0: strcpy_P(statusText, (const char *)F("REC UNREAD")); break;
      case 1: strcpy_P(statusText, (const char *)F("REC READ")); break;
      case 2: strcpy_P(statusText, (const char *)F("STO UNSENT")); break;
      case 3: strcpy_P(statusText, (const char *)F("STO SENT")); break;
      case 4: strcpy_P(statusText, (const char *)F("ALL")); break;
    }
  	sprintf(buffer, "%s\"%s\",%d\r", command, statusText, mode);
  }

	dte->clearReceivedBuffer();
	dte->ATCommand(buffer);
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(strcat(buffer, "\r\n"))) return false;
	}
  if(!dte->ATResponse()) return false;
	while(!dte->isResponse(F("OK\r\n"))) {
    if(dte->isResponseContain(F("+CMGL: "))) {
    	char *pointer = strstr(dte->getResponse(), "+CMGL: ") + strlen_P((const char *)F("+CMGL: "));
    	char *str = strtok(pointer, ",\r\n");
      for (unsigned char i = 0; i < 7 && str != NULL; i++) {
        if(i == 0) message.index = str[0] - '0';
        if(i == 1) {
          str++; str[strlen(str)-1] = '\0';
          message.status = messageStatusIndex(str);
          if(message.status == 0 || message.status == 2)
            if(mode == 0)
              message.status++;
        }
        if(i == 2) {
          if(str[0] == '"') {
            str++; str[strlen(str)-1] = '\0';
            strcpy(message.address, str);
          }
          else
            message.firstOctet = atoi(str);
        }
        if(i == 3) {
          if(message.firstOctet != 0)
            message.mr = atoi(str);
        }
        if(i == 4) {
          if(str[0] == '"') {
            str++; str[strlen(str)-1] = '\0';
            strcpy(message.timestamp, str);
          }
        }
        str = strtok(NULL, ",\r\n");
      }
      strcpy(message.data, "");
      if(strlen(message.address) > 0) {
        if(!dte->ATResponse()) return false;
        while(!dte->isResponse(F("OK\r\n")) && !dte->isResponseContain(F("+CMGL: "))) {
          if(dte->getResponse()[0] == '+')
            urc.unsolicitedResultCode(dte->getResponse());
          else strcat(message.data, dte->getResponse());
          if(!dte->ATResponse()) return false;
        }
      }
      this->message[message.index - 1] = message;
    }
    else if(!dte->ATResponse()) return false;
  }
  return true;
}

bool SMS::atReadSMS(unsigned char index, unsigned char mode) {
  return false;
}

bool SMS::atSendSMS(const char destination[], const char message[]) {
	char buffer[strlen_P((const char *)F("AT+CMGS=\"\",XXX\r\r\n")) + strlen(destination) + 1];

	sprintf(buffer, "AT+CMGS=\"%s\"\r", destination);

	dte->clearReceivedBuffer();
	dte->ATCommand(buffer);
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(strcat(buffer, "\r\n"))) return false;
	}
	if(!dte->ATResponseContain(F(">"))) return false;
  dte->ATCommand(message);
  dte->ATCommand("\x1A");
	if(!dte->ATResponseContain(F("\x1A"))) return false;
	if(!dte->ATResponseContain(F("+CMGS: "), 60000)) return false;
	if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
	return true;
}
