/*
*
*/
#ifndef HTTP_h
#define HTTP_h

#include <Arduino.h>
#include <DTE.h>

class HTTP
{
private:
  DTE *dte;
  unsigned char statusMethod = 0;
  unsigned char status = 0;
  char dataReceived[203];

  void initializeVariable(void);
  unsigned char methodIndex(const char method[]);

public:
  HTTP(DTE &dte);

  bool atInitializeHTTPService(void);
  bool atTerminateHTTPService(void);
  bool atSetHTTPParametersValue(const char paramTag[], const char paramValue[]);
  bool atSetHTTPParametersValue(const __FlashStringHelper paramTag[], const char paramValue[]);
  bool atInputHTTPData(const char data[], unsigned int timeout = 2000);
  bool atHTTPMethodAction(unsigned char method);
  bool atReadHTTPServerResponse(unsigned long startAddress = 0, unsigned long byteSize = 0);
  bool atReadHTTPStatus(void);

  bool init(unsigned char timeout = 30);
  bool paramUserAgent(const char userAgent[]);
  bool paramContentType(const char contentType[]);
  bool actionGet(const char url[]);
  bool actionPost(const char url[], const char data[]);
  bool actionHead(const char url[]);
  bool isIdle(void);
  const char *readDataReceived(void);
  bool terminate(void);

  unsigned char getStatusMethod(void) { return statusMethod; }
  unsigned char getStatus(void) { return status; }

  bool isHTTPActionUpdated(void);
  unsigned int getHTTPActionStatusCode(void);
  unsigned int getHTTPActionDataLength(void);
};

#endif
