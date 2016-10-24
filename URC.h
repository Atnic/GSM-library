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
  };
  struct EnterPin {
    bool updated = false;
    char code[11];
  };
  struct HttpAction {
    bool updated = false;
    unsigned char method;
    unsigned int statusCode;
    unsigned int dataLength;
  };
  struct Psuttz {
    bool updated = false;
    unsigned int year;
    unsigned char month;
    unsigned char day;
    unsigned char hour;
    unsigned char minute;
    unsigned char second;
    unsigned char timezone;
    unsigned char dst;
  };
  struct NewMessageIndication {
    bool updated = false;
    char mem[3];
    unsigned char index;
  };


  /** Get Local Timestamp Unsolicited Result Code */
  struct Psuttz psuttz;

  /** Call Ready Unsolicited Result Code */
  struct CallReady callReady;

  /** +CPIN Unsolicited Result Code */
  struct EnterPin enterPin;

  /** +HTTPACTION Unsolicited Result Code */
  struct HttpAction httpAction;

  /** +CMTI Unsolicited Result Code */
  struct NewMessageIndication newMessageIndication;

	URC();

  /**
   * Unsolicited Result Code (URC) check, if it URC,
   * then update URC Object member value
   * @param  urc Unsolicited Result Code string
   * @return     true: If it is URC, false: If it is not
   */
	bool unsolicitedResultCode(const char urc[]);

  /** Reset URC Object member value. This should be called if Module Reset */
  void resetUnsolicitedResultCode(void);
};

extern URC Urc;

#endif
