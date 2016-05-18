#include "SIMCOM.h"

/* SIMCOM Class */
SIMCOM::SIMCOM(DTE &dte)
{
	this->dte = &dte;
}

bool SIMCOM::atPowerOff(bool normal) {
	char buffer[strlen_P((const char *)F("AT+CPOWD=X\r\r\n")) + 1];

	sprintf(buffer, "AT+CPOWD=%d\r", (normal)?1:0);

	dte->clearReceivedBuffer();
  dte->ATCommand(buffer);
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(strcat(buffer, "\r\n"))) return false;
	}
  if(normal) {
    if(!dte->ATResponseEqual(F("NORMAL POWER DOWN\r\n"), 5000)) return false;
  }
	return true;
}

bool SIMCOM::atIndicateRIWhenUsingURC(void) {
	dte->clearReceivedBuffer();
  dte->ATCommand(F("AT+CFGRI?\r"));
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(F("AT+CFGRI?\r\r\n"))) return false;
	}
	if(!dte->ATResponseContain(F("+CFGRI: "))) return false;
	char *pointer = strstr(dte->getResponse(), "+CFGRI: ") + strlen_P((const char *)F("+CFGRI: "));
	char *str = strtok(pointer, "\r\n");
	if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
  indicateRI = (str[0] == '1')?true:false;
	return true;
}

bool SIMCOM::atIndicateRIWhenUsingURC(bool indicate) {
	char buffer[strlen_P((const char *)F("AT+CFGRI=X\r\r\n")) + 1];

	sprintf(buffer, "AT+CFGRI=%d\r", (indicate)?1:0);

	dte->clearReceivedBuffer();
  dte->ATCommand(buffer);
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(strcat(buffer, "\r\n"))) return false;
	}
  if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
	this->indicateRI = indicate;
	return true;
}

bool SIMCOM::atLocalTimestap(bool enable) {
	char buffer[strlen_P((const char *)F("AT+CLTS=X\r\r\n")) + 1];

	sprintf(buffer, "AT+CLTS=%d\r", (enable)?1:0);

	dte->clearReceivedBuffer();
  dte->ATCommand(buffer);
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(strcat(buffer, "\r\n"))) return false;
	}
  if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
	this->localTimestampEnabled = enable;
	return true;
}

bool SIMCOM::atSimInsertedStatusReporting(void) {
  char n = '0';
  char inserted = '0';

	dte->clearReceivedBuffer();
  dte->ATCommand(F("AT+CSMINS?\r"));
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(F("AT+CSMINS?\r\r\n"))) return false;
	}
	if(!dte->ATResponseContain(F("+CSMINS: "))) return false;
	char *pointer = strstr(dte->getResponse(), "+CSMINS: ") + strlen_P((const char *)F("+CSMINS: "));
	char *str = strtok(pointer, ",\r\n");
	int i = 0;
	for (i = 0; i < 2 && str != NULL; i++) {
	  if(i == 0) n = str[0];
		if(i == 1) inserted = str[0];
		str = strtok(NULL, ",\r\n");
	}
	if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
  simInsertedURCEnabled = (n == '1')?true:false;
  simInserted = (inserted == '1')?true:false;
	return true;
}

bool SIMCOM::atConfigureSlowClock(void) {
	dte->clearReceivedBuffer();
  dte->ATCommand(F("AT+CSCLK?\r"));
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(F("AT+CSCLK?\r\r\n"))) return false;
	}
	if(!dte->ATResponseContain(F("+CSCLK: "))) return false;
	char *pointer = strstr(dte->getResponse(), "+CSCLK: ") + strlen_P((const char *)F("+CSCLK: "));
	char *str = strtok(pointer, "\r\n");
	if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
  slowClockMode = str[0] - '0';
	return true;
}

bool SIMCOM::atConfigureSlowClock(unsigned char mode) {
	char buffer[strlen_P((const char *)F("AT+CSCLK=X\r\r\n")) + 1];

	sprintf(buffer, "AT+CSCLK=%d\r", mode);

	dte->clearReceivedBuffer();
  dte->ATCommand(buffer);
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(strcat(buffer, "\r\n"))) return false;
	}
  if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
  slowClockMode = mode;
	return true;
}

bool SIMCOM::atDeleteAllSMS(unsigned char type) {
	char buffer[strlen_P((const char *)F("AT+CMGDA=\r\r\n")) + 13];
  char command[] = "AT+CMGDA=";

  if(dte->isPDU()) {
  	sprintf(buffer, "%s%d\r", command, type);
  }
  else {
    char typeText[7];
    switch (type) {
      case 1: strcpy_P(typeText, (const char *)F("READ")); break;
      case 2: strcpy_P(typeText, (const char *)F("UNREAD")); break;
      case 3: strcpy_P(typeText, (const char *)F("SENT")); break;
      case 4: strcpy_P(typeText, (const char *)F("UNSENT")); break;
      case 5: strcpy_P(typeText, (const char *)F("INBOX")); break;
      case 6: strcpy_P(typeText, (const char *)F("ALL")); break;
    }
  	sprintf(buffer, "%s\"DEL %s\"\r", command, typeText);
  }

	dte->clearReceivedBuffer();
  dte->ATCommand(buffer);
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(strcat(buffer, "\r\n"))) return false;
	}
  if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
	return true;
}

bool SIMCOM::atUnstructuredSupplementaryServiceData(void) {
	dte->clearReceivedBuffer();
  dte->ATCommand(F("AT+CUSD?\r"));
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(F("AT+CUSD?\r\r\n"))) return false;
	}
	if(!dte->ATResponseContain(F("+CUSD: "))) return false;
	char *pointer = strstr(dte->getResponse(), "+CUSD: ") + strlen_P((const char *)F("+CUSD: "));
	char *str = strtok(pointer, "\r\n");
	if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
  ussdResultCodePresentation = str[0] - '0';
	return true;
}

bool SIMCOM::atUnstructuredSupplementaryServiceData(unsigned char n, const char str[], unsigned char dcs) {
	char buffer[strlen_P((const char *)F("AT+CUSD=X,\"XXXXXXXXXXXXXXXX\",XX\r\r\n")) + 1];
  char command[] = "AT+CUSD=";

  if(n == 2 && ussdStatus < 2)
    sprintf(buffer, "%s%d\r", command, n);
	else if(n == 1 && strlen(str) == 0)
    sprintf(buffer, "%s%d\r", command, n);
  else if(n < 2) {
    if(dcs == 0)
      sprintf(buffer, "%s%d,\"%s\"\r", command, n, str);
    else
      sprintf(buffer, "%s%d,\"%s\",%d\r", command, n, str, dcs);
  }
  else
    return false;

	if(n == 2) {
		ussdStatus = 0;
	}

	dte->clearReceivedBuffer();
  dte->ATCommand(buffer);
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(strcat(buffer, "\r\n"))) return false;
	}
  if(!dte->ATResponseEqual(F("OK\r\n"), 1500)) return false;
  if(n == 1) {
    if(!dte->ATResponseContain(F("+CUSD: "), 5000)) return false;
		char *pointer = strstr(dte->getResponse(), "+CUSD: ") + strlen_P((const char *)F("+CUSD: "));
		char *str = strtok(pointer, ",\r\n");
	  ussdStatus = str[0] - '0';
		return true;
	}
  return true;
}

bool SIMCOM::atUnstructuredSupplementaryServiceData(unsigned char n, const __FlashStringHelper str[], unsigned char dcs) {
	char buffer[strlen_P((const char *)str)+1];
	strcpy_P(buffer, (const char *)str);
	return atUnstructuredSupplementaryServiceData(n, buffer, dcs);
}
