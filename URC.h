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

  /** Call Ready Unsolicited Result Code */
  struct CallReady callReady;

  /** +CPIN Unsolicited Result Code */
  struct EnterPin enterPin;

  /** +HTTPACTION Unsolicited Result Code */
  struct HttpAction httpAction;

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

extern URC urc;

#endif
