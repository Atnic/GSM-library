#include "TCPIP.h"
#include "URC.h"

unsigned char TCPIP::connectionStatusIndex(const char *status) {
  if(strcmp_P(status, (const char *)F("IP INITIAL")) == 0) return 0;
  if(strcmp_P(status, (const char *)F("IP START")) == 0) return 1;
  if(strcmp_P(status, (const char *)F("IP CONFIG")) == 0) return 2;
  if(strcmp_P(status, (const char *)F("IP GPRSACT")) == 0) return 3;
  if(strcmp_P(status, (const char *)F("IP STATUS")) == 0) return 4;
  if(strcmp_P(status, (const char *)F("TCP CONNECTING")) == 0) return 5;
  if(strcmp_P(status, (const char *)F("UDP CONNECTING")) == 0) return 5;
  if(strcmp_P(status, (const char *)F("SERVER LISTENING")) == 0) return 5;
  if(strcmp_P(status, (const char *)F("CONNECT OK")) == 0) return 6;
  if(strcmp_P(status, (const char *)F("TCP CLOSING")) == 0) return 7;
  if(strcmp_P(status, (const char *)F("UDP CLOSING")) == 0) return 7;
  if(strcmp_P(status, (const char *)F("TCP CLOSED")) == 0) return 8;
  if(strcmp_P(status, (const char *)F("UDP CLOSED")) == 0) return 8;
  if(strcmp_P(status, (const char *)F("PDP DEACT")) == 0) return 9;
  return 0;
}

/* TCPIP Class */
TCPIP::TCPIP(DTE &dte) {
	this->dte = &dte;
}

bool TCPIP::atStartUpMultiIPConnection(void) {
	dte->clearReceivedBuffer();
  dte->ATCommand(F("AT+CIPMUX?\r"));
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(F("AT+CIPMUX?\r\r\n"))) return false;
	}
	if(!dte->ATResponseContain(F("+CIPMUX: "))) return false;
	char *pointer = strstr(dte->getResponse(), "+CIPMUX: ") + strlen_P((const char *)F("+CIPMUX: "));
	char *str = strtok(pointer, "\r\n");
	if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
  multiIPConnection = (str[0] == '1')?true:false;
	return true;
}

bool TCPIP::atStartUpMultiIPConnection(bool multi) {
	char buffer[strlen_P((const char *)F("AT+CIPMUX=X\r\r\n")) + 1];

	sprintf(buffer, "AT+CIPMUX=%d\r", (multi)?1:0);

	dte->clearReceivedBuffer();
  dte->ATCommand(buffer);
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(strcat(buffer, "\r\n"))) return false;
	}
  if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
	multiIPConnection = multi;
	return true;
}

bool TCPIP::atStartUpTCPUDPConnection(const char mode[], const char host[], unsigned int port) {
	return atStartUpTCPUDPConnection(0, mode, host, port);
}

bool TCPIP::atStartUpTCPUDPConnection(unsigned char n, const char mode[], const char host[], unsigned int port) {
  char buffer[strlen_P((const char *)F("AT+CIPSTART=X,\"\",\"\",\"\"\r\r\n")) + strlen(mode) + strlen(host) + 4 + 1];

  if(!multiIPConnection)
    sprintf(buffer, "AT+CIPSTART=\"%s\",\"%s\",%d\r", mode, host, port);
  else
    sprintf(buffer, "AT+CIPSTART=%d,\"%s\",\"%s\",%d\r", n, mode, host, port);

	dte->clearReceivedBuffer();
  dte->ATCommand(buffer);
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(strcat(buffer, "\r\n"))) return false;
	}
  if(!dte->ATResponse()) return false;
  if(!dte->isResponse(F("OK\r\n")) && !dte->isResponse(F("ERROR\r\n"))) {
    urc.unsolicitedResultCode(dte->getResponse());
    return false;
  }
  if(!dte->ATResponse(7500)) return false;
  if(dte->isResponse(F("ALREADY CONNECT\r\n")))
    return true;
  else if(dte->isResponse(F("CONNECT OK\r\n")))
    return true;
  else if(dte->isResponseContain(F("STATE: "))) {
    char *pointer = strstr(dte->getResponse(), "STATE: ") + strlen_P((const char *)F("STATE: "));
    char *str = strtok(pointer, "\"\r\n");
    if(!dte->ATResponseEqual(F("CONNECT FAIL\r\n"))) return false;
    connectionStatus = connectionStatusIndex(str);
    return true;
  }
  else {
    urc.unsolicitedResultCode(dte->getResponse());
    return false;
  }
}

bool TCPIP::atDeactiveGPRSPDPContext(void) {
	dte->clearReceivedBuffer();
  dte->ATCommand(F("AT+CIPSHUT\r"));
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(F("AT+CIPSHUT\r\r\n"))) return false;
	}
	if(!dte->ATResponseEqual(F("SHUT OK\r\n"), 5000)) return false;
	return true;
}

bool TCPIP::atQueryCurrentConnectionStatus(void) {
	dte->clearReceivedBuffer();
  dte->ATCommand(F("AT+CIPSTATUS\r"));
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(F("AT+CIPSTATUS\r\r\n"))) return false;
	}
  if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
  if(!dte->ATResponseContain(F("STATE: "))) return false;
  char *pointer = strstr(dte->getResponse(), "STATE: ") + strlen_P((const char *)F("STATE: "));
  char *str = strtok(pointer, "\"\r\n");
  connectionStatus = connectionStatusIndex(str);
  if(multiIPConnection) {
    for (unsigned char i = 0; i < 8; i++) {
      if(!dte->ATResponseContain(F("C: "))) return false;
    }
  }
  return true;
}
