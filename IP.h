/*
*
*/
#ifndef IP_h
#define IP_h

#include <Arduino.h>
#include <DTE.h>

class IP
{
private:
  DTE *dte;
  struct BearerProfile {
    unsigned char cid;
    struct ConnParam {
      char contype[5];
      char apn[15];
      char phonenum[5];
      char user[15];
      char pwd[15];
      unsigned char rate;
    } connPara;
    struct ConnStatus {
      unsigned char status;
      char ip[16];
    } connStatus;
  } bearerProfile[3];

  void initializeVariable(void);

public:
  IP(DTE &dte);

  bool atBearerSettings(unsigned char cmdType, unsigned char cid, const char paramTag[] = "", const char paramValue[] = "");

  unsigned char getConnectionStatus(unsigned char cid) { return bearerProfile[cid-1].connStatus.status; };
};

#endif
