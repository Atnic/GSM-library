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
  struct HTTTPAction {
    bool updated = true;
    unsigned char method;
    unsigned int statusCode;
    unsigned int dataLength;
  } HTTPAction;

	URC();

	bool unsolicitedResultCode(const char urc[]);
};

extern URC urc;

#endif
