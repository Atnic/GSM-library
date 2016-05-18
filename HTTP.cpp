#include "HTTP.h"
#include "URC.h"

void HTTP::initializeVariable(void) {
  statusMethod = 0;
  status = 0;
}

unsigned char HTTP::methodIndex(const char method[]) {
  if(strcmp_P(method, (const char *)F("GET")) == 0) return 0;
  if(strcmp_P(method, (const char *)F("POST")) == 0) return 1;
  if(strcmp_P(method, (const char *)F("HEAD")) == 0) return 2;
	return 0;
}

/* HTTP Class */
HTTP::HTTP(DTE &dte)
{
	this->dte = &dte;
  initializeVariable();
}

bool HTTP::atInitializeHTTPService(void) {
	dte->clearReceivedBuffer();
  dte->ATCommand(F("AT+HTTPINIT\r"));
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(F("AT+HTTPINIT\r\r\n"))) return false;
	}
	if(!dte->ATResponseEqual(F("OK\r\n"))) { initializeVariable(); return false; }
	return true;
}

bool HTTP::atTerminateHTTPService(void) {
	dte->clearReceivedBuffer();
  dte->ATCommand(F("AT+HTTPTERM\r"));
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(F("AT+HTTPTERM\r\r\n"))) return false;
	}
	if(!dte->ATResponseEqual(F("OK\r\n"))) { initializeVariable(); return false; }
	return true;
}

bool HTTP::atSetHTTPParametersValue(const char paramTag[], const char paramValue[]) {
	char buffer[strlen_P((const char *)F("AT+HTTPPARA=\"\",\"\"\r\r\n")) + strlen(paramTag) + strlen(paramValue) + 1];

	sprintf(buffer, "AT+HTTPPARA=\"%s\",\"%s\"\r", paramTag, paramValue);

	dte->clearReceivedBuffer();
  dte->ATCommand(buffer);
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(strcat(buffer, "\r\n"))) return false;
	}
	if(!dte->ATResponseEqual(F("OK\r\n"))) { initializeVariable(); return false; }
	return true;
}

bool HTTP::atSetHTTPParametersValue(const __FlashStringHelper paramTag[], const char paramValue[]) {
	char buffer[strlen_P((const char *)paramTag)+1];
	strcpy_P(buffer, (const char *)paramTag);
	return atSetHTTPParametersValue(buffer, paramValue);
}

bool HTTP::atInputHTTPData(const char data[], unsigned int timeout) {
	char buffer[strlen_P((const char *)F("AT+HTTPDATA=XXXX,XXXXX\r\r\n")) + 1];

	sprintf(buffer, "AT+HTTPDATA=%d,%d\r", strlen(data), timeout);

	dte->clearReceivedBuffer();
  dte->ATCommand(buffer);
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(strcat(buffer, "\r\n"))) return false;
	}
	if(!dte->ATResponseEqual(F("DOWNLOAD\r\n"))) { initializeVariable(); return false; }
  dte->ATCommand(data);
	if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
	return true;
}

bool HTTP::atHTTPMethodAction(unsigned char method) {
	char buffer[strlen_P("AT+HTTPACTION=X\r\r\n") + 1];

	sprintf(buffer, "AT+HTTPACTION=%d\r", method);

  urc.HTTPAction.updated = false;
	dte->clearReceivedBuffer();
  dte->ATCommand(buffer);
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(strcat(buffer, "\r\n"))) return false;
	}
	if(!dte->ATResponseEqual(F("OK\r\n"))) { initializeVariable(); return false; }
	return true;
}

bool HTTP::atReadHTTPServerResponse(unsigned long startAddress, unsigned long byteSize) {
	char buffer[strlen_P("AT+HTTPREAD=XXXXXX,XXXXXX\r\r\n") + 1];
	char command[] = "AT+HTTPREAD";
  char dataReceived[205];

	unsigned int dataLength = 0;

	if((byteSize == 0l) || (200l < byteSize))
		byteSize = 200l;
	sprintf(buffer, "%s=%ld,%ld\r", command, startAddress, byteSize);

	dte->clearReceivedBuffer();
  dte->ATCommand(buffer);
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(strcat(buffer, "\r\n"))) return false;
	}
  if(dte->ATResponse()) {
    if(dte->isResponse("ERROR\r\n")) { initializeVariable(); return false; }
    if(dte->isResponse("OK\r\n")) {
    	strcpy(dataReceived, "");
      strcpy(this->dataReceived, dataReceived);
      return true;
    }
  	if(!dte->isResponseContain(F("+HTTPREAD:"))) {
      urc.unsolicitedResultCode(dte->getResponse());
      return false;
    }
  }
	char *pointer = strstr(dte->getResponse(), "+HTTPREAD:") + strlen_P((const char *)F("+HTTPREAD:"));
	char *str = strtok(pointer, "\r\n");
	dataLength = atoi(str);
  if(dataLength > 0) {
    if(!dte->ATResponse()) return false;
    strcpy(dataReceived, "");
    while (!dte->isResponse(F("OK\r\n"))) {
      strcat(dataReceived, dte->getResponse());
      if(!dte->ATResponse()) return false;
    }
    strcpy(this->dataReceived, dataReceived);
    return true;
  }
  else {
    if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
  	return true;
  }
}

bool HTTP::atReadHTTPStatus(void) {
	char mode[5];
	unsigned char status = 0;
	// unsigned int finish;
	// unsigned int remain;

	dte->clearReceivedBuffer();
  dte->ATCommand(F("AT+HTTPSTATUS?\r"));
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(F("AT+HTTPSTATUS?\r\r\n"))) return false;
	}
	if(!dte->ATResponseContain(F("+HTTPSTATUS: "))) { initializeVariable(); return false; }
	char *pointer = strstr(dte->getResponse(), "+HTTPSTATUS: ") + strlen_P((const char *)F("+HTTPSTATUS: "));
	char *str = strtok(pointer, ",\r\n");
  unsigned char i = 0;
  for (i = 0; i < 4 && str != NULL; i++) {
    if(i == 0) strcpy(mode, str);
    if(i == 1) status = str[0] - '0';
    // if(i == 2) finish = atoi(str);
    // if(i == 3) remain = atoi(str);
    str = strtok(NULL, ",\r\n");
  }
	if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
	this->statusMethod = methodIndex(mode);
	this->status = status;
	return true;
}

bool HTTP::init(unsigned char timeout) {
	char buffer[4];

	if(!atInitializeHTTPService()) return false;
	itoa(timeout, buffer, 10);
	return atSetHTTPParametersValue(F("TIMEOUT"), buffer);
}

bool HTTP::paramUserAgent(const char userAgent[]) {
	return atSetHTTPParametersValue(F("UA"), userAgent);
}

bool HTTP::paramContentType(const char contentType[]) {
	return atSetHTTPParametersValue(F("CONTENT"), contentType);
}

bool HTTP::actionGet(const char url[]) {
	if(!isIdle()) return false;
	if(!atSetHTTPParametersValue(F("URL"), url)) return false;
	if(!atHTTPMethodAction(0)) return false;
	return true;
}

bool HTTP::actionPost(const char url[], const char data[]) {
	if(!isIdle()) return false;
	if(!atSetHTTPParametersValue(F("URL"), url)) return false;
	if(!atInputHTTPData(data)) return false;
	if(!atHTTPMethodAction(1)) return false;
	return true;
}

bool HTTP::actionHead(const char url[]) {
	if(!isIdle()) return false;
	if(!atSetHTTPParametersValue(F("URL"), url)) return false;
	if(!atHTTPMethodAction(2)) return false;
	return true;
}

bool HTTP::isIdle(void) {
	if(!atReadHTTPStatus()) return false;
	if(status != 0) return false;
	return true;
}

const char *HTTP::readDataReceived(void) {
	if(!atReadHTTPServerResponse()) return "";
	return dataReceived;
}

bool HTTP::terminate(void) {
	return atTerminateHTTPService();
}

bool HTTP::isHTTPActionUpdated(void) {
	return urc.HTTPAction.updated;
}

unsigned int HTTP::getHTTPActionStatusCode(void) {
	return urc.HTTPAction.statusCode;
}

unsigned int HTTP::getHTTPActionDataLength(void) {
	return urc.HTTPAction.dataLength;
}
