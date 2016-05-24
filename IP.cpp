#include "IP.h"

/* IP Class */
IP::IP(DTE &dte, GPRS &gprs)
{
	this->dte = &dte;
	this->gprs = &gprs;
}

bool IP::atBearerSettings(unsigned char cmdType, unsigned char cid, const char paramTag[], const char paramValue[]) {
	char buffer[22 + strlen(paramTag) + strlen(paramValue)];  // "AT+SAPBR=X,X,\"{paramTag}\",\"{paramValue}\"\r\r\n"
	struct BearerProfile bearerProfile;

	if(cmdType <= 5 && cmdType != 3) {
    const __FlashStringHelper *command = F("AT+SAPBR=%d,%d\r");
		sprintf_P(buffer, (const char *)command, cmdType, cid);
	}
	else if(cmdType == 3) {
    const __FlashStringHelper *command = F("AT+SAPBR=%d,%d,\"%s\",\"%s\"\r");
		sprintf_P(buffer, (const char *)command, cmdType, cid, paramTag, paramValue);
	}
	else
		return false;

  __FlashStringHelper *response = F("+SAPBR: ");

  bearerProfile = this->bearerProfile[cid-1];
	dte->clearReceivedBuffer();
  if(!dte->ATCommand(buffer)) return false;
	if(cmdType == 2) {
		if(!dte->ATResponseContain(response)) return false;
		char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
		char *str = strtok(pointer, ",\"");
		for (unsigned char i = 0; i < 3 && str != NULL; i++) {
			if(i == 0) bearerProfile.cid = str[0] - '0';
			if(i == 1) bearerProfile.connStatus.status = str[0] - '0';
			if(i == 2) strcpy(bearerProfile.connStatus.ip, str);
			str = strtok(NULL, ",\"");
		}
		if(!dte->ATResponseOk()) return false;
		this->bearerProfile[cid-1] = bearerProfile;
	}
	else if(cmdType == 4) {
		if(!dte->ATResponseContain(response)) return false;
		for (unsigned char i = 0; i < 6; i++) {
			if(!dte->ATResponse()) return false;
			if(dte->isResponseContain("CONTYPE: ")) {
				response = F("CONTYPE: ");
				char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
				char *str = strtok(pointer, ",\"");
				strcpy(bearerProfile.connParam.contype, str);
			}
			else if (dte->isResponseContain("APN: ")) {
				response = F("APN: ");
				char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
				char *str = strtok(pointer, ",\"");
				strcpy(bearerProfile.connParam.apn, str);
			}
			else if (dte->isResponseContain("APN: ")) {
				response = F("APN: ");
				char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
				char *str = strtok(pointer, ",\"");
				strcpy(bearerProfile.connParam.apn, str);
			}
		}
		if(!dte->ATResponseOk()) return false;
		bearerProfile[cid-1] = bearerProfile;
	}
	else if(!dte->ATResponseOk(5000)) return false;
	return true;
}

void IP::setConnectionParamGprs(const char apn[], const char user[], const char pwd[], unsigned char cid) {
	atBearerSettings(3, cid, "APN", apn);
	atBearerSettings(3, cid, "USER", user);
	atBearerSettings(3, cid, "PWD", pwd);
	atBearerSettings(4, cid);
}

struct ConnStatus IP::getConnectionStatus(unsigned char cid) {
	atBearerSettings(2, cid);
	return bearerProfile[cid-1].connStatus;
}

struct ConnParam IP::getConnectionParam(unsigned char cid) {
	atBearerSettings(4, cid);
	return bearerProfile[cid-1].connParam;
}
