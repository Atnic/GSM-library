#include "HTTP.h"
#include "URC.h"

unsigned char HTTP::methodIndex(const char method[]) {
  if(strcmp_P(method, (const char *)F("GET")) == 0) return 0;
  if(strcmp_P(method, (const char *)F("POST")) == 0) return 1;
  if(strcmp_P(method, (const char *)F("HEAD")) == 0) return 2;
  return 0;
}

bool HTTP::atInitializeHttpService(void) {
  const __FlashStringHelper *command = F("AT+HTTPINIT\r");

  dte->clearReceivedBuffer();
  if(!dte->ATCommand(command)) return false;
  if(!dte->ATResponseOk()) return false;
  initialized = true;
  return true;
}

bool HTTP::atTerminateHttpService(void) {
  const __FlashStringHelper *command = F("AT+HTTPTERM\r");

  initialized = false;
  dte->clearReceivedBuffer();
  if(!dte->ATCommand(command)) return false;
  if(!dte->ATResponseOk()) return false;
  return true;
}

bool HTTP::atSetHttpParametersValue(const char paramTag[], const char paramValue[], const char userdataDelimiter[]) {
  const __FlashStringHelper *command = F("AT+HTTPPARA=\"%s\",\"%s\"\r");
  if (strlen(userdataDelimiter) > 0) command = F("AT+HTTPPARA=\"%s\",\"%s\",\"%s\"\r");
  char buffer[24 + + strlen(paramTag) + strlen(paramValue) + strlen(userdataDelimiter)]; ///"AT+HTTPPARA=\"{paramTag}\",\"{paramValue}\"\r\r\n"

  if (strlen(userdataDelimiter) == 0)
    sprintf_P(buffer, (const char *)command, paramTag, paramValue);
  else
    sprintf_P(buffer, (const char *)command, paramTag, paramValue, userdataDelimiter);

  dte->clearReceivedBuffer();
  if(!dte->ATCommand(buffer)) return false;
  if(!dte->ATResponseOk()) return false;
  return true;
}

bool HTTP::atSetHttpParametersValue(const __FlashStringHelper paramTag[], const char paramValue[], const char userdataDelimiter[]) {
  char buffer[strlen_P((const char *)paramTag)+1];
  strcpy_P(buffer, (const char *)paramTag);
  return atSetHttpParametersValue(buffer, paramValue, userdataDelimiter);
}

bool HTTP::atInputHttpData(const char data[], unsigned int timeout) {
  const __FlashStringHelper *command = F("AT+HTTPDATA=%u,%d\r");
  const __FlashStringHelper *response = F("DOWNLOAD");
  char buffer[26]; // "AT+HTTPDATA={strlen(data)},{timeout}}\r\r\n"

  sprintf_P(buffer, (const char *)command, strlen(data), timeout);

  dte->clearReceivedBuffer();
  if(!dte->ATCommand(buffer)) return false;
  if(!dte->ATResponseEqual(response)) return false;
  size_t i = 0;
  unsigned long t = millis();
  while(i < strlen(data)) {
    i += dte->write(data);
    if(millis() - t > (timeout*1000)) return false;
  }
  if(!dte->ATResponseOk()) return false;
  return true;
}

bool HTTP::atHttpMethodAction(unsigned char method) {
  const __FlashStringHelper *command = F("AT+HTTPACTION=%d\r");
  char buffer[19]; // "AT+HTTPACTION={method}\r\r\n"

  sprintf_P(buffer, (const char *)command, method);

  Urc.httpAction.updated = false;
  Urc.httpAction.method = method;
  Urc.httpAction.dataLength = 0;
  Urc.httpAction.statusCode = 0;
  dte->clearReceivedBuffer();
  if(!dte->ATCommand(buffer)) return false;
  if(!dte->ATResponseOk(1000)) return false;
  return true;
}

bool HTTP::atReadHttpServerResponse(char dataReceived[], unsigned long startAddress, unsigned long byteSize) {
  char buffer[29]; // "AT+HTTPREAD=XXXXXX,XXXXXX\r\r\n"
  unsigned long serverResponseDataLength = 0;

  if(byteSize > 0) {
    const __FlashStringHelper *command = F("AT+HTTPREAD=%ld,%ld\r");
    sprintf_P(buffer, (const char *)command, startAddress, byteSize);
  }
  else return false;

  const __FlashStringHelper *response = F("+HTTPREAD:");

  dte->clearReceivedBuffer();
  if(!dte->ATCommand(buffer)) return false;
  if(dte->ATResponse()) {
    if(dte->isResponseContain("ERROR")) return false;
    if(dte->isResponseOk()) {
      this->serverResponseDataLength = 0;
      strcpy(dataReceived, "");
      return true;
    }
    if(!dte->isResponseContain(response)) {
      Urc.unsolicitedResultCode(dte->getResponse());
      return false;
    }
  }
  char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
  char *str = strtok(pointer, "\r\n");
  serverResponseDataLength = atoi(str);
  this->serverResponseDataLength = serverResponseDataLength;
  if(byteSize > serverResponseDataLength)
  byteSize = serverResponseDataLength;
  if(byteSize > 0)
  dte->readBytes(dataReceived, byteSize);
  dataReceived[byteSize] = '\0';
  if(!dte->ATResponseOk()) return false;
  return true;
}

bool HTTP::atReadHttpStatus(void) {
  const __FlashStringHelper *command = F("AT+HTTPSTATUS?\r");
  const __FlashStringHelper *response = F("+HTTPSTATUS: ");
  struct HttpStatus httpStatus;

  dte->clearReceivedBuffer();
  if(!dte->ATCommand(command)) return false;
  if(!dte->ATResponseContain(response)) return false;
  char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
  char *str = strtok(pointer, ",");
  for (unsigned char i = 0; i < 4 && str != NULL; i++) {
    if(i == 0) strcpy(httpStatus.mode, str);
    if(i == 1) httpStatus.status = str[0] - '0';
    if(i == 2) httpStatus.finish = atol(str);
    if(i == 3) httpStatus.remain = atol(str);
    str = strtok(NULL, ",");
  }
  if(!dte->ATResponseOk()) return false;
  this->httpStatus = httpStatus;
  return true;
}

bool HTTP::atSslHttp(void) {
  const __FlashStringHelper *command = F("AT+HTTPSSL\r");

  dte->clearReceivedBuffer();
  if(!dte->ATCommand(command)) return false;
  if(!dte->ATResponseOk()) return false;
  sslEnabled = true;
  return true;
}

bool HTTP::atSslHttp(bool enable) {
  const __FlashStringHelper *command = F("AT+HTTPSSL=%d\r");
  char buffer[19]; // "AT+HTTPSSL={enable}\r\r\n"

  sprintf_P(buffer, (const char *)command, enable? 1 : 0);

  dte->clearReceivedBuffer();
  if(!dte->ATCommand(buffer)) return false;
  if(!dte->ATResponseOk()) return false;
  sslEnabled = enable;
  return true;
}

/* HTTP Class */
HTTP::HTTP(DTE &dte, IP &ip)
{
  this->dte = &dte;
  this->ip = &ip;
  this->initialized = false;
  this->httpStatus = (struct HttpStatus) { "GET", 0, 0 };
  this->serverResponseDataLength = 0;
}

struct HttpStatus HTTP::getStatus(void) {
  atReadHttpStatus();
  return httpStatus;
}

bool HTTP::initialize(unsigned int timeout, unsigned char cid) {
  if (ip->getConnectionStatus(cid).status == 1) {
    if(initialized) atTerminateHttpService();
    if(!initialized) {
      if(!atInitializeHttpService()) return false;
      char buffer[5];
      if(timeout < 30) timeout = 30;
      if(!atSetHttpParametersValue(F("TIMEOUT"), itoa(timeout, buffer, 10))) return false;
      if(!atSetHttpParametersValue(F("CONTENT"), "application/x-www-form-urlencoded")) return false;
      if(!atSetHttpParametersValue(F("REDIR"), "1")) return false;
    }
  }
  else {
    if(initialized) atTerminateHttpService();
  }
  return initialized;
}

bool HTTP::setUserAgent(const char userAgent[]) {
  if(!initialized) return false;
  atSetHttpParametersValue(F("UA"), userAgent);
  return true;
}

bool HTTP::setHeaders(const char headers[], const char userdataDelimiter[]) {
  if(!initialized) return false;
  atSetHttpParametersValue(F("USERDATA"), headers, userdataDelimiter);
  return true;
}

bool HTTP::action(const char method[], const char url[], const char data[]) {
  if(!initialized) return false;
  if(!getStatus().status == 0) return false;
  if(strstr_P(url, (const char *)F("https://")) != NULL) {
    if (!atSslHttp(true)) return false;
  }
  else {
    if (!atSslHttp(false)) return false;
  }
  if(!atSetHttpParametersValue(F("URL"), url)) return false;
  if((methodIndex(method) == 1) && strlen(data) > 0) {
    if(!atInputHttpData(data)) return false;
  }
  if(!atHttpMethodAction(methodIndex(method))) return false;
  if(Urc.httpAction.updated && Urc.httpAction.statusCode == 603) {
    if((methodIndex(method) == 1) && strlen(data) > 0) {
      if(!atInputHttpData(data)) return false;
    }
    if(!atHttpMethodAction(methodIndex(method))) return false;
  }
  return true;
}

bool HTTP::readDataReceived(char buffer[], unsigned long length, unsigned long startAddress) {
  if(!initialized) return false;
  if(!getStatus().status == 0) return false;
  if(Urc.httpAction.statusCode != 0) {
    if(!Urc.httpAction.updated) return false;
  }
  if(!atReadHttpServerResponse(buffer, startAddress, length)) return false;
  return true;
}

bool HTTP::terminate(void) {
  if(initialized) atTerminateHttpService();
  return !initialized;
}
