#include "IP.h"

void IP::initializeVariable(void) {
  for (unsigned char i = 0; i < 3; i++) {
  	bearerProfile[i].connStatus.status = 3;
  }
}

/* IP Class */
IP::IP(DTE &dte)
{
	this->dte = &dte;
  initializeVariable();
}

bool IP::atBearerSettings(unsigned char cmdType, unsigned char cid, const char paramTag[], const char paramValue[]) {
	char buffer[strlen_P((const char *)F("AT+SAPBR=X,X\r\r\n")) + strlen(paramTag) + 3 + strlen(paramValue) + 3 + 1];
	char command[] = "AT+SAPBR=";

	struct BearerProfile bearerProfile;

	if(cmdType <= 5 && cmdType != 3) {
		sprintf(buffer, "%s%d,%d\r", command, cmdType, cid);
	}
	else if(cmdType == 3) {
		sprintf(buffer, "%s%d,%d,\"%s\",\"%s\"\r", command, cmdType, cid, paramTag, paramValue);
	}
	else
		return false;

	dte->clearReceivedBuffer();
  dte->ATCommand(buffer);
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(strcat(buffer, "\r\n"))) return false;
	}
	if(cmdType == 2) {
		if(!dte->ATResponseContain(F("+SAPBR: "))) { initializeVariable(); return false; }
		char *pointer = strstr(dte->getResponse(), "+SAPBR: ") + strlen_P((const char *)F("+SAPBR: "));
		char *str = strtok(pointer, ",\r\n");
		unsigned char i = 0;
		for (i = 0; i < 3 && str != NULL; i++) {
			if(i == 0) bearerProfile.cid = str[0] - '0';
			if(i == 1) bearerProfile.connStatus.status = str[0] - '0';
			if(i == 2) strcpy(bearerProfile.connStatus.ip, str);
			str = strtok(NULL, ",\"\r\n");
		}
		if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
		this->bearerProfile[bearerProfile.cid - 1] = bearerProfile;
	}
	else if(cmdType == 4) {
		if(!dte->ATResponseContain(F("+SAPBR:"))) { initializeVariable(); return false; }
		// Not implemented yet
		if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
	}
	else if(!dte->ATResponseEqual(F("OK\r\n"), 5000)) { initializeVariable(); return false; }
	return true;
}
