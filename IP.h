/*
*
*/
#ifndef IP_h
#define IP_h

#include <Arduino.h>
#include <DTE.h>
#include <GPRS.h>

struct ConnStatus {
  unsigned char status = 3;
  char ip[16] = "";
};

struct ConnParam {
  char contype[5] = "";
  char apn[15] = "";
  char phonenum[5] = "";
  char user[15] = "";
  char pwd[15] = "";
  unsigned char rate = 2;
};

struct BearerProfile {
  unsigned char cid = 1;
  struct ConnStatus connStatus;
  struct ConnParam connParam;
};

class IP
{
private:
  DTE *dte;
  GPRS *gprs;
  struct BearerProfile bearerProfile[3];

  bool atBearerSettings(unsigned char cmdType, unsigned char cid, const char paramTag[] = "", const char paramValue[] = "");

public:
  IP(DTE &dte, GPRS &gprs);

  void setConnectionParamGprs(const char apn[], const char user[], const char pwd[]);

  struct ConnStatus getConnectionStatus(unsigned char cid = 1);
  struct ConnParam getConnectionParam(unsigned char cid = 1);
};

#endif
