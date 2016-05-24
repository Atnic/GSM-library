/*
 *
 */
#ifndef URC_h
#define URC_h

#include <Arduino.h>

// Library Interface Description
class URC
{
private:

protected:

public:
  struct CallReady {
    bool updated = false;
  } callReady;
  struct EnterPin {
    bool updated = false;
    char code[11];
  } enterPin;
  struct HttpAction {
    bool updated = false;
    unsigned char method;
    unsigned int statusCode;
    unsigned int dataLength;
  } httpAction;

	URC();

	bool unsolicitedResultCode(const char urc[]);
  void resetUnsolicitedResultCode(void);
};

extern URC urc;

#endif
