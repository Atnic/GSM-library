#include "GPRS.h"
#include "URC.h"

void GPRS::initializeVariable(void) {
	attached = false;
}

/* GPRS Class */
GPRS::GPRS(DTE &dte)
{
	this->dte = &dte;
	initializeVariable();
}

bool GPRS::atAttachGPRSService(void) {
	dte->clearReceivedBuffer();
  dte->ATCommand(F("AT+CGATT?\r"));
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(F("AT+CGATT?\r\r\n"))) return false;
	}
	if(!dte->ATResponseContain(F("+CGATT: "))) { initializeVariable(); return false; }
	char *pointer = strstr(dte->getResponse(), "+CGATT: ") + strlen_P((const char *)F("+CGATT: "));
	char *str = strtok(pointer, "\r\n");
	if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
  attached = (str[0] == '1')?true:false;
	return true;
}

bool GPRS::atAttachGPRSService(bool attach) {
	char buffer[strlen_P((const char *)F("AT+CGATT=X\r\r\n")) + 1];

	sprintf(buffer, "AT+CGATT=%d\r", (attach)?1:0);

	dte->clearReceivedBuffer();
  dte->ATCommand(buffer);
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(strcat(buffer, "\r\n"))) return false;
	}
  if(!dte->ATResponseEqual(F("OK\r\n"), 10000)) { initializeVariable(); return false; }
	this->attached = attach;
	return true;
}

bool GPRS::atDefinePDPContext(void) {
  unsigned char index = 0;
  struct PDPContext pdpContext[3];

	dte->clearReceivedBuffer();
  dte->ATCommand(F("AT+CGDCONT?\r"));
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(F("AT+CGDCONT?\r\r\n"))) return false;
	}
  if(!dte->ATResponse()) return false;
  while(!dte->isResponse(F("OK\r\n"))) {
  	if(dte->isResponseContain(F("+CGDCONT: "))) {
      char *pointer = strstr(dte->getResponse(), "+CGDCONT: ") + strlen_P((const char *)F("+CGDCONT: "));
    	char *str = strtok(pointer, ",\r\n");
      unsigned char i;
      for (i = 0; i < 6 && str != NULL; i++) {
        if(i == 0) {
          index = str[0] - 1 - '0';
          pdpContext[index].contextIdentifier = str[0] - '0';
        }
        if(i == 1) strcpy(pdpContext[index].type, str);
        if(i == 2) strcpy(pdpContext[index].apn, str);
        if(i == 3) strcpy(pdpContext[index].address, str);
        str = strtok(NULL, ",\r\n");
      }
      strcpy(pdpContext[index].type, strtok(pdpContext[index].type, "\""));
      strcpy(pdpContext[index].apn, strtok(pdpContext[index].apn, "\""));
      strcpy(pdpContext[index].address, strtok(pdpContext[index].address, "\""));
    }
		else if(dte->isResponse(F("ERROR\r\n"))) { initializeVariable(); return false; }
		else urc.unsolicitedResultCode(dte->getResponse());
    if(!dte->ATResponse()) return false;
  }
  memcpy(this->pdpContext, pdpContext, 3 * sizeof(struct PDPContext));
	return true;
}

bool GPRS::atDefinePDPContext(unsigned char cid, const char *type, const char *apn, const char *address) {
	char buffer[strlen_P((const char *)F("AT+CGDCONT=X\r\r\n")) + 5 + 14 + 17 + 1];

  if(type == NULL)
    sprintf(buffer, "AT+CGDCONT=%d\r", cid);
  else
    sprintf(buffer, "AT+CGDCONT=%d,\"%s\",\"%s\",\"%s\"\r", cid, type, apn, address);

	dte->clearReceivedBuffer();
  dte->ATCommand(buffer);
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(strcat(buffer, "\r\n"))) return false;
	}
  if(!dte->ATResponseEqual(F("OK\r\n"))) { initializeVariable(); return false; }
  pdpContext[(cid-1)].contextIdentifier = cid;
  strcpy(pdpContext[(cid-1)].type, type);
  strcpy(pdpContext[(cid-1)].apn, apn);
  strcpy(pdpContext[(cid-1)].address, address);
	return true;
}

bool GPRS::atNetworkRegistrationStatus(void) {
  struct NetworkRegistration networkRegistration;

	dte->clearReceivedBuffer();
  dte->ATCommand(F("AT+CGREG?\r"));
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(F("AT+CGREG?\r\r\n"))) return false;
	}
	if(!dte->ATResponseContain(F("+CGREG: "))) return false;
	char *pointer = strstr(dte->getResponse(), "+CGREG: ") + strlen_P((const char *)F("+CGREG: "));
	char *str = strtok(pointer, ",\"\r\n");
  unsigned char i = 0;
  for (i = 0; i < 4 && str != NULL; i++) {
    if(i == 0) networkRegistration.mode = str[0] - '0';
    if(i == 1) networkRegistration.status = str[0] - '0';
    if(i == 2) strcpy(networkRegistration.locationAreaCode, str);
    if(i == 3) strcpy(networkRegistration.cellID, str);
    str = strtok(NULL, ",\"\r\n");
  }
	if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
  this->networkRegistration.mode = networkRegistration.mode;
  this->networkRegistration.status = networkRegistration.status;
  if(i > 2) {
    strcpy(this->networkRegistration.locationAreaCode, networkRegistration.locationAreaCode);
    strcpy(this->networkRegistration.cellID, networkRegistration.cellID);
  }
	return true;
}

bool GPRS::atNetworkRegistrationStatus(unsigned char n) {
	char buffer[strlen_P((const char *)F("AT+CGREG=X\r\r\n")) + 1];

	sprintf(buffer, "AT+CGREG=%d\r", n);

	dte->clearReceivedBuffer();
  dte->ATCommand(buffer);
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(strcat(buffer, "\r\n"))) return false;
	}
  if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
	return true;
}
