#ifndef IP_h
#define IP_h

#include <Arduino.h>
#include "DTE.h"
#include "GPRS.h"

struct ConnStatus {
  unsigned char status;
  char ip[16];
};

struct ConnParam {
  char contype[5];
  char apn[15];
  char phonenum[5];
  char user[15];
  char pwd[15];
  unsigned char rate;
};

struct BearerProfile {
  unsigned char cid;
  struct ConnStatus connStatus;
  struct ConnParam connParam;
};

class IP {
 private:
  DTE *dte;
  GPRS *gprs;
  struct BearerProfile bearerProfile[3];

 public:
  IP(DTE &dte, GPRS &gprs);

  /**
   * Command AT+SABPR
   * @param  cmdType    Command type 1 - 5
   *                    0: Close bearer
   *                    1: Open bearer
   *                    2: Query bearer
   *                    3: Set bearer parameter
   *                    4: Get bearer parameter
   *                    5: Save the value of parameter to NVRAM
   * @param  cid        Context Identifier
   * @param  paramTag   Parameter Tag for command type 3
   *                    "CONTYPE": GPRS or CSD
   *                    "APN": Access Point Name
   *                    "USER": User Name
   *                    "PWD": Password
   *                    "PHONENUM": Phone Number for CSD Call
   *                    "CONRATE": CSD Connection Rate
   * @param  paramValue Parameter Value
   * @return            true: If command successful, false: Otherwise
   */
  bool atBearerSettings(unsigned char cmdType, unsigned char cid, const char paramTag[] = "", const char paramValue[] = "");
  bool atBearerSettings(unsigned char cmdType, unsigned char cid, const __FlashStringHelper *paramTag, const char paramValue[] = "");
  bool atBearerSettings(unsigned char cmdType, unsigned char cid, const char paramTag[], const __FlashStringHelper *paramValue);
  bool atBearerSettings(unsigned char cmdType, unsigned char cid, const __FlashStringHelper *paramTag, const __FlashStringHelper *paramValue);

  /**
   * Set GPRS Connection Parameter.
   * @param apn  Access Point Name
   * @param user User Name, default: No username
   * @param pwd  Password, default: No password
   * @param cid  Context Identifier, default: 1
   */
  void setConnectionParamGprs(const char apn[], const char user[] = "", const char pwd[] = "", unsigned char cid = 1);
  void setConnectionParamGprs(const __FlashStringHelper *apn, const char user[] = "", const char pwd[] = "", unsigned char cid = 1);
  void setConnectionParamGprs(const char apn[], const __FlashStringHelper *user, const char pwd[] = "", unsigned char cid = 1);
  void setConnectionParamGprs(const __FlashStringHelper *apn, const __FlashStringHelper *user, const char pwd[] = "", unsigned char cid = 1);
  void setConnectionParamGprs(const char apn[], const char user[], const __FlashStringHelper *pwd, unsigned char cid = 1);
  void setConnectionParamGprs(const __FlashStringHelper *apn, const char user[], const __FlashStringHelper *pwd, unsigned char cid = 1);
  void setConnectionParamGprs(const char apn[], const __FlashStringHelper *user, const __FlashStringHelper *pwd, unsigned char cid = 1);
  void setConnectionParamGprs(const __FlashStringHelper *apn, const __FlashStringHelper *user, const __FlashStringHelper *pwd, unsigned char cid = 1);

  /**
   * Get Connection status in Struct ConnStatus.
   * @param  cid Context identifier, default: 1
   * @return     ConnStatus Struct
   */
  struct ConnStatus getConnectionStatus(unsigned char cid = 1);

  /**
   * Get Connection parameter in Struct ConnParam.
   * Connection parameter can be save on 3 storage, which is identified by CID
   * @param  cid Context Identifier, default: 1
   * @return     ConnParam Struct
   */
  struct ConnParam getConnectionParam(unsigned char cid = 1);

  /**
   * Open connection, register to network, get IP Addresses
   * @param  cid Context identifier, default: 1
   * @return     true: If success, false: Otherwise
   */
  bool openConnection(unsigned char cid = 1);

  /**
   * Close connection, unregister to network.
   * @param  cid Context identifier, default: 1
   * @return     true: If success, false: Otherwise
   */
  bool closeConnection(unsigned char cid = 1);
};

#endif
