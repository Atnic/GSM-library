#include "IP.h"

/* IP Class */
IP::IP(DTE &dte, GPRS &gprs) {
  this->dte = &dte;
  this->gprs = &gprs;
  bearerProfile[0] = (struct BearerProfile){1, (struct ConnStatus){3, ""}, (struct ConnParam){"", "", "", "", "", 2}};
  bearerProfile[1] = (struct BearerProfile){2, (struct ConnStatus){3, ""}, (struct ConnParam){"", "", "", "", "", 2}};
  bearerProfile[2] = (struct BearerProfile){3, (struct ConnStatus){3, ""}, (struct ConnParam){"", "", "", "", "", 2}};
}

bool IP::atBearerSettings(unsigned char cmdType, unsigned char cid, const char paramTag[], const char paramValue[]) {
  char buffer[20 + strlen(paramTag) + strlen(paramValue)];  // "AT+SAPBR=X,X,\"{paramTag}\",\"{paramValue}\"\r"
  struct BearerProfile bearerProfile;

  if (cmdType <= 5 && cmdType != 3) {
    const __FlashStringHelper *command = F("AT+SAPBR=%d,%d\r");
    sprintf_P(buffer, (const char *)command, cmdType, cid);
  } else if (cmdType == 3) {
    const __FlashStringHelper *command = F("AT+SAPBR=%d,%d,\"%s\",\"%s\"\r");
    sprintf_P(buffer, (const char *)command, cmdType, cid, paramTag, paramValue);
  } else
    return false;

  const __FlashStringHelper *response = F("+SAPBR:");

  bearerProfile = this->bearerProfile[cid - 1];
  dte->clearReceivedBuffer();
  if (!dte->ATCommand(buffer)) return false;
  if (cmdType == 2) {
    if (!dte->ATResponseContain(response)) return false;
    char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
    char *str = strtok(pointer, ",\"");
    for (unsigned char i = 0; i < 3 && str != NULL; i++) {
      if (i == 0) bearerProfile.cid = str[0] - '0';
      if (i == 1) bearerProfile.connStatus.status = str[0] - '0';
      if (i == 2) strcpy(bearerProfile.connStatus.ip, str);
      str = strtok(NULL, ",\"");
    }
    if (!dte->ATResponseOk()) return false;
    this->bearerProfile[cid - 1] = bearerProfile;
  } else if (cmdType == 4) {
    if (!dte->ATResponseContain(response)) return false;
    for (unsigned char i = 0; i < 6; i++) {
      if (!dte->ATResponse()) return false;
      if (dte->isResponseContain(F("CONTYPE: "))) {
        const __FlashStringHelper *response = F("CONTYPE: ");
        char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
        char *str = strtok(pointer, ",\"");
        strcpy(bearerProfile.connParam.contype, str);
      } else if (dte->isResponseContain(F("APN: "))) {
        const __FlashStringHelper *response = F("APN: ");
        char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
        char *str = strtok(pointer, ",\"");
        strcpy(bearerProfile.connParam.apn, str);
      } else if (dte->isResponseContain(F("USER: "))) {
        const __FlashStringHelper *response = F("USER: ");
        char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
        char *str = strtok(pointer, ",\"");
        strcpy(bearerProfile.connParam.user, str);
      } else if (dte->isResponseContain(F("PWD: "))) {
        const __FlashStringHelper *response = F("PWD: ");
        char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
        char *str = strtok(pointer, ",\"");
        strcpy(bearerProfile.connParam.pwd, str);
      } else if (dte->isResponseContain(F("PHONENUM: "))) {
        const __FlashStringHelper *response = F("PHONENUM: ");
        char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
        char *str = strtok(pointer, ",\"");
        strcpy(bearerProfile.connParam.phonenum, str);
      } else if (dte->isResponseContain(F("RATE: "))) {
        const __FlashStringHelper *response = F("RATE: ");
        char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
        char *str = strtok(pointer, ",\"");
        bearerProfile.connParam.rate = str[0] - '0';
      }
    }
    if (!dte->ATResponseOk()) return false;
    this->bearerProfile[cid - 1] = bearerProfile;
  } else if (!dte->ATResponseOk(10000))
    return false;
  return true;
}

bool IP::atBearerSettings(unsigned char cmdType, unsigned char cid, const __FlashStringHelper *paramTag, const char paramValue[]) {
  char buffer[strlen_P((const char *)paramTag) + 1];
  strcpy_P(buffer, (const char *)paramTag);
  return atBearerSettings(cmdType, cid, buffer, paramValue);
}

bool IP::atBearerSettings(unsigned char cmdType, unsigned char cid, const char paramTag[], const __FlashStringHelper *paramValue) {
  char buffer[strlen_P((const char *)paramValue) + 1];
  strcpy_P(buffer, (const char *)paramValue);
  return atBearerSettings(cmdType, cid, paramTag, buffer);
}

bool IP::atBearerSettings(unsigned char cmdType, unsigned char cid, const __FlashStringHelper *paramTag, const __FlashStringHelper *paramValue) {
  char buffer[strlen_P((const char *)paramValue) + 1];
  strcpy_P(buffer, (const char *)paramValue);
  return atBearerSettings(cmdType, cid, paramTag, buffer);
}

void IP::setConnectionParamGprs(const char apn[], const char user[], const char pwd[], unsigned char cid) {
  struct ConnParam connParam = this->bearerProfile[cid - 1].connParam;
  bool change = false;

  if (strcmp_P(connParam.contype, (const char *)F("GPRS")) != 0) {
    atBearerSettings(3, cid, F("CONTYPE"), F("GPRS"));
    change = true;
  }
  if (strcmp_P(connParam.apn, apn) != 0) {
    atBearerSettings(3, cid, F("APN"), apn);
    change = true;
  }
  if (strcmp_P(connParam.user, user) != 0) {
    atBearerSettings(3, cid, F("USER"), user);
    change = true;
  }
  if (strcmp_P(connParam.pwd, pwd) != 0) {
    atBearerSettings(3, cid, F("PWD"), pwd);
    change = true;
  }
  if (change) {
    atBearerSettings(5, 1);
    getConnectionParam(cid);
  }
}

void IP::setConnectionParamGprs(const __FlashStringHelper *apn, const char user[], const char pwd[], unsigned char cid) {
  char buffer[strlen_P((const char *)apn) + 1];
  strcpy_P(buffer, (const char *)apn);
  setConnectionParamGprs(buffer, user, pwd, cid);
}

void IP::setConnectionParamGprs(const char apn[], const __FlashStringHelper *user, const char pwd[], unsigned char cid) {
  char buffer[strlen_P((const char *)user) + 1];
  strcpy_P(buffer, (const char *)user);
  setConnectionParamGprs(apn, buffer, pwd, cid);
}

void IP::setConnectionParamGprs(const __FlashStringHelper *apn, const __FlashStringHelper *user, const char pwd[], unsigned char cid) {
  char buffer[strlen_P((const char *)user) + 1];
  strcpy_P(buffer, (const char *)user);
  setConnectionParamGprs(apn, buffer, pwd, cid);
}

void IP::setConnectionParamGprs(const char apn[], const char user[], const __FlashStringHelper *pwd, unsigned char cid) {
  char buffer[strlen_P((const char *)pwd) + 1];
  strcpy_P(buffer, (const char *)pwd);
  setConnectionParamGprs(apn, user, buffer, cid);
}

void IP::setConnectionParamGprs(const __FlashStringHelper *apn, const char user[], const __FlashStringHelper *pwd, unsigned char cid) {
  char buffer[strlen_P((const char *)pwd) + 1];
  strcpy_P(buffer, (const char *)pwd);
  setConnectionParamGprs(apn, user, buffer, cid);
}

void IP::setConnectionParamGprs(const char apn[], const __FlashStringHelper *user, const __FlashStringHelper *pwd, unsigned char cid) {
  char buffer[strlen_P((const char *)pwd) + 1];
  strcpy_P(buffer, (const char *)pwd);
  setConnectionParamGprs(apn, user, buffer, cid);
}

void IP::setConnectionParamGprs(const __FlashStringHelper *apn, const __FlashStringHelper *user, const __FlashStringHelper *pwd, unsigned char cid) {
  char buffer[strlen_P((const char *)pwd) + 1];
  strcpy_P(buffer, (const char *)pwd);
  setConnectionParamGprs(apn, user, buffer, cid);
}

struct ConnStatus IP::getConnectionStatus(unsigned char cid) {
  atBearerSettings(2, cid);
  return bearerProfile[cid - 1].connStatus;
}

struct ConnParam IP::getConnectionParam(unsigned char cid) {
  atBearerSettings(4, cid);
  return bearerProfile[cid - 1].connParam;
}

bool IP::openConnection(unsigned char cid) {
  struct ConnStatus connStatus = getConnectionStatus(cid);

  if (connStatus.status == 3) {
    if (!gprs->isAttached()) return false;
    if (!atBearerSettings(1, cid)) return false;
    connStatus = getConnectionStatus(cid);
  }
  if (connStatus.status >= 2) return false;
  return true;
}

bool IP::closeConnection(unsigned char cid) {
  struct ConnStatus connStatus = getConnectionStatus(cid);

  if (connStatus.status == 1) {
    if (!atBearerSettings(0, cid)) return false;
    connStatus = getConnectionStatus(cid);
  }
  if (connStatus.status <= 1) return false;
  return true;
}
