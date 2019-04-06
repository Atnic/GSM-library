#include "GSM.h"
#include "URC.h"

/* GSM Class */
GSM::GSM(DTE &dte) {
  this->dte = &dte;
  selectedOperator = (struct Operator){0, 0, "", ""};
  phonebookMemoryStorage = (struct PhonebookMemoryStorage){"", 0, 0};
  networkRegistration = (struct NetworkRegistration){0, 4, "", ""};
  signalQuality = (struct SignalQuality){0, 0};
  clock = (struct Clock){"", 0, 0, 0, 0, 0, 0, 0};
  subscriberNumber = (struct SubscriberNumber){"", "", 0, 0, 0};
  batteryStatus = (struct BatteryStatus){false, 0, 0.0f};
}

bool GSM::atOperatorSelection(void) {
  const __FlashStringHelper *command = F("AT+COPS?\r");
  const __FlashStringHelper *response = F("+COPS: ");
  struct Operator selectedOperator;

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(command)) return false;
  if (!dte->ATResponseContain(response)) return false;
  char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
  char *str = strtok(pointer, ",\"");
  selectedOperator = this->selectedOperator;
  for (unsigned char i = 0; i < 3 && str != NULL; i++) {
    if (i == 0) selectedOperator.mode = str[0] - '0';
    if (i == 1) selectedOperator.format = str[0] - '0';
    if (i == 2) {
      if (selectedOperator.format == 2)
        strcpy(selectedOperator.operNumeric, str);
      else
        strcpy(selectedOperator.oper, str);
    }
    str = strtok(NULL, ",\"");
  }
  if (!dte->ATResponseOk()) return false;
  this->selectedOperator = selectedOperator;
  return true;
}

bool GSM::atOperatorSelection(unsigned char mode, unsigned char format, const char oper[]) {
  char buffer[16 + strlen(oper)];  // "AT+COPS=X,X,\"{oper}\"\r"

  if (mode == 0) {
    const __FlashStringHelper *command = F("AT+COPS=%d\r");
    sprintf_P(buffer, (const char *)command, mode);
  } else if ((mode == 1 || mode == 4) && format < 3 && strlen(oper) > 0) {
    const __FlashStringHelper *command = F("AT+COPS=%d,%d,\"%s\"\r");
    sprintf_P(buffer, (const char *)command, mode, format, oper);
  } else if (mode == 3 && format < 3) {
    const __FlashStringHelper *command = F("AT+COPS=%d,%d\r");
    sprintf_P(buffer, (const char *)command, mode, format);
  } else
    return false;

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(buffer)) return false;
  if (!dte->ATResponseOk()) return false;
  return true;
}

bool GSM::atOperatorSelection(unsigned char mode, unsigned char format, const __FlashStringHelper *oper) {
  char buffer[strlen_P((const char *)oper) + 1];
  strcpy_P(buffer, (const char *)oper);
  return atOperatorSelection(mode, format, buffer);
}

bool GSM::atSelectPhonebookMemoryStorage(void) {
  const __FlashStringHelper *command = F("AT+CPBS?\r");
  const __FlashStringHelper *response = F("+CPBS: ");
  struct PhonebookMemoryStorage phonebookMemoryStorage;

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(command)) return false;
  if (!dte->ATResponseContain(response)) return false;
  char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
  char *str = strtok(pointer, ",\"");
  for (unsigned char i = 0; i < 3 && str != NULL; i++) {
    if (i == 0) strcpy(phonebookMemoryStorage.storage, str);
    if (i == 1) phonebookMemoryStorage.used = atoi(str);
    if (i == 2) phonebookMemoryStorage.total = atoi(str);
    str = strtok(NULL, ",\"");
  }
  if (!dte->ATResponseOk()) return false;
  this->phonebookMemoryStorage = phonebookMemoryStorage;
  return true;
}

bool GSM::atSelectPhonebookMemoryStorage(const char storage[]) {
  const __FlashStringHelper *command = F("AT+CPBS=\"%s\"\r");
  char buffer[12 + strlen(storage)];  // "AT+CPBS=\"{storage}\"\r"

  sprintf_P(buffer, (const char *)command, storage);

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(buffer)) return false;
  if (!dte->ATResponseOk()) return false;
  return true;
}

bool GSM::atWritePhonebookEntry(unsigned char index, const char phoneNumber[]) {
  const __FlashStringHelper *command = F("AT+CPBW=%d,\"%s\"\r");
  char buffer[12 + strlen(phoneNumber)];

  sprintf_P(buffer, (const char *)command, index, phoneNumber);

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(buffer)) return false;
  if (!dte->ATResponseOk()) return false;
  return true;
}

bool GSM::atSelectPhonebookMemoryStorage(const __FlashStringHelper *storage) {
  char buffer[strlen_P((const char *)storage) + 1];
  strcpy_P(buffer, (const char *)storage);
  return atSelectPhonebookMemoryStorage(buffer);
}

bool GSM::atEnterPIN(void) {
  const __FlashStringHelper *command = F("AT+CPIN?\r");
  const __FlashStringHelper *response = F("+CPIN: ");

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(command)) return false;
  if (!dte->ATResponseContain(response, 1000)) return false;
  char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
  char *str = strtok(pointer, "\"");
  if (!dte->ATResponseOk()) return false;
  strcpy(pinStatus, str);
  return true;
}

bool GSM::atNetworkRegistration(void) {
  const __FlashStringHelper *command = F("AT+CGREG?\r");
  const __FlashStringHelper *response = F("+CGREG: ");
  struct NetworkRegistration networkRegistration;

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(command)) return false;
  if (!dte->ATResponseContain(response)) return false;
  char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
  char *str = strtok(pointer, ",\"");
  for (unsigned char i = 0; i < 4 && str != NULL; i++) {
    if (i == 0) networkRegistration.n = atoi(str);
    if (i == 1) networkRegistration.status = atoi(str);
    if (i == 2) strcpy(networkRegistration.lac, str);
    if (i == 3) strcpy(networkRegistration.ci, str);
    str = strtok(NULL, ",\"");
  }
  if (!dte->ATResponseOk()) return false;
  this->networkRegistration = networkRegistration;
  return true;
}

bool GSM::atNetworkRegistration(unsigned char n) {
  const __FlashStringHelper *command = F("AT+CGREG=%d\r");
  char buffer[12];  // "AT+CGREG=X\r"

  sprintf_P(buffer, (const char *)command, n);

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(buffer)) return false;
  if (!dte->ATResponseOk()) return false;
  return true;
}

bool GSM::atSignalQualityReport(void) {
  const __FlashStringHelper *command = F("AT+CSQ\r");
  const __FlashStringHelper *response = F("+CSQ: ");
  struct SignalQuality signalQuality;

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(command)) return false;
  if (!dte->ATResponseContain(response)) return false;
  char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
  char *str = strtok(pointer, ",");
  for (unsigned char i = 0; i < 2 && str != NULL; i++) {
    if (i == 0) signalQuality.rssi = atoi(str);
    if (i == 1) signalQuality.ber = atoi(str);
    str = strtok(NULL, ",");
  }
  if (!dte->ATResponseOk()) return false;
  this->signalQuality = signalQuality;
  return true;
}

bool GSM::atSubscriberNumber(void) {
  const __FlashStringHelper *command = F("AT+CNUM\r");
  const __FlashStringHelper *response = F("+CNUM:");
  struct SubscriberNumber subscriberNumber;

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(command)) return false;
  while (true) {
    if (!dte->ATResponse()) return false;
    if (dte->isResponseOk()) return true;
    if (dte->isResponseContain(response)) break;
    dte->unsolicitedResultCode();
  }
  char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
  char *str = strtok(pointer, ",");
  for (unsigned char i = 0; i < 5 && str != NULL; i++) {
    if (i == 0) strcpy(subscriberNumber.characterSet, str);
    if (i == 1) strcpy(subscriberNumber.number, str);
    if (i == 2) subscriberNumber.type = atoi(str);
    if (i == 3) subscriberNumber.speed = str[0] - '0';
    if (i == 4) subscriberNumber.service = str[0] - '0';
    str = strtok(NULL, ",");
  }
  strcpy(subscriberNumber.characterSet, strtok(subscriberNumber.characterSet, " \""));
  strcpy(subscriberNumber.number, strtok(subscriberNumber.number, "\""));
  if (!dte->ATResponseOk()) return false;
  this->subscriberNumber = subscriberNumber;
  return true;
}

bool GSM::atClock(void) {
  const __FlashStringHelper *command = F("AT+CCLK?\r");
  const __FlashStringHelper *response = F("+CCLK: ");

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(command)) return false;
  if (!dte->ATResponseContain(response)) return false;
  char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
  char *str = strtok(pointer, "\"");
  if (!dte->ATResponseOk()) return false;
  strcpy(clock.timestamp, str);
  str = strtok(str, "/,:+");
  for (unsigned char i = 0; i < 7 && str != NULL; i++) {
    if (i == 0) clock.year = atoi(str);
    if (i == 1) clock.month = atoi(str);
    if (i == 2) clock.day = atoi(str);
    if (i == 3) clock.hour = atoi(str);
    if (i == 4) clock.minute = atoi(str);
    if (i == 5) clock.second = atoi(str);
    if (i == 6) clock.timezone = atoi(str) / 4;
    str = strtok(NULL, "/,:+");
  }
  return true;
}

bool GSM::atClock(const char timestamp[]) {
  const __FlashStringHelper *command = F("AT+CCLK=\"%s\"\r");
  char buffer[12 + strlen(timestamp)];  // "AT+CCLK=\"{timestamp}\"\r"

  sprintf_P(buffer, (const char *)command, timestamp);

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(buffer)) return false;
  if (!dte->ATResponseOk()) return false;
  return true;
}

bool GSM::atClock(const __FlashStringHelper *timestamp) {
  char buffer[strlen_P((const char *)timestamp) + 1];
  strcpy_P(buffer, (const char *)timestamp);
  return atClock(buffer);
}

bool GSM::atBatteryCharge(void) {
  const __FlashStringHelper *command = F("AT+CBC\r");
  const __FlashStringHelper *response = F("+CBC: ");
  struct BatteryStatus batteryStatus;

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(command)) return false;
  if (!dte->ATResponseContain(response)) return false;
  char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
  char *str = strtok(pointer, ",");
  unsigned char i = 0;
  for (i = 0; i < 3 && str != NULL; i++) {
    if (i == 0) batteryStatus.charge = (str[0] == '1') ? true : false;
    if (i == 1) batteryStatus.capacityLevel = atoi(str);
    if (i == 2) batteryStatus.voltage = (float)atoi(str) * 0.001;
    str = strtok(NULL, ",");
  }
  if (!dte->ATResponseOk()) return false;
  this->batteryStatus = batteryStatus;
  return true;
}

bool GSM::atUnstructuredSupplementaryServiceData(void) {
  const __FlashStringHelper *command = F("AT+CUSD?\r");
  const __FlashStringHelper *response = F("+CUSD: ");

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(command)) return false;
  if (!dte->ATResponseContain(response)) return false;
  char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
  char *str = strtok(pointer, " ");
  serviceData.n = atoi(str);
  if (!dte->ATResponseOk()) return false;

  return true;
}

bool GSM::atUnstructuredSupplementaryServiceData(unsigned char n) {
  char buffer[11];  // "AT+CUSD=X\r"
  const __FlashStringHelper *command = F("AT+CUSD=%d\r");
  sprintf_P(buffer, (const char *)command, n);

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(buffer)) return false;
  if (!dte->ATResponseOk()) return false;
  return true;
}

bool GSM::atUnstructuredSupplementaryServiceData(unsigned char n, const char str[]) {
  char buffer[14 + strlen(str)];  // "AT+CUSD=1,\"{str}\"\r"
  const __FlashStringHelper *command = F("AT+CUSD=%d,\"%s\"\r");
  sprintf_P(buffer, (const char *)command, n, str);

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(buffer)) return false;
  if (!dte->ATResponseOk()) return false;
  return true;
}

bool GSM::atUnstructuredSupplementaryServiceData(unsigned char n, const __FlashStringHelper *str) {
  char buffer[strlen_P((const char *)str) + 1];
  strcpy_P(buffer, (const char *)str);
  return atUnstructuredSupplementaryServiceData(n, buffer);
}

bool GSM::atUnstructuredSupplementaryServiceData(unsigned char n, const char str[], unsigned char dcs) {
  char buffer[17 + strlen(str)];  // "AT+CUSD=1,\"{str}\",XX\r"
  const __FlashStringHelper *command = F("AT+CUSD=%d,\"%s\",%d\r");
  sprintf_P(buffer, (const char *)command, n, str, dcs);

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(buffer)) return false;
  if (!dte->ATResponseOk()) return false;
  return true;
}

bool GSM::atUnstructuredSupplementaryServiceData(unsigned char n, const __FlashStringHelper *str, unsigned char dcs) {
  char buffer[strlen_P((const char *)str) + 1];
  strcpy_P(buffer, (const char *)str);
  return atUnstructuredSupplementaryServiceData(n, buffer, dcs);
}

struct Operator GSM::getOperator(unsigned char format) {
  if (strlen(selectedOperator.operNumeric) == 0) {
    if (getNetworkRegistration().status == 1 || getNetworkRegistration().status == 5) {
      atOperatorSelection(3, 2);
      atOperatorSelection();
    }
  }
  if (format >= 2)
    format = 0;
  if (format < 2) {
    if (selectedOperator.format != format || strlen(selectedOperator.oper) == 0) {
      if (getNetworkRegistration().status == 1 || getNetworkRegistration().status == 5) {
        atOperatorSelection(3, format);
        atOperatorSelection();
      }
    }
  }
  return selectedOperator;
}

struct PhonebookMemoryStorage GSM::getPhonebookMemoryStrorage(void) {
  if (getNetworkRegistration().status == 1 || getNetworkRegistration().status == 5)
    atSelectPhonebookMemoryStorage();
  return phonebookMemoryStorage;
}

const char *GSM::getPinStatus(void) {
  atEnterPIN();
  return pinStatus;
}

struct NetworkRegistration GSM::getNetworkRegistration(void) {
  atNetworkRegistration();
  return networkRegistration;
}

struct SignalQuality GSM::getSignalQuality(void) {
  atSignalQualityReport();
  return signalQuality;
}

struct Clock GSM::getClock(void) {
  static unsigned long t = 0;
  if (millis() - t > 500) {
    if (strcmp(getPinStatus(), "READY") == 0) {
      t = millis();
      atClock();
    }
  }
  return clock;
}

struct SubscriberNumber GSM::getSubscriberNumber(void) {
  if (strlen(subscriberNumber.number) == 0)
    if (getNetworkRegistration().status == 1 || getNetworkRegistration().status == 5)
      atSubscriberNumber();
  return subscriberNumber;
}

struct BatteryStatus GSM::getBatteryStatus(void) {
  atBatteryCharge();
  return batteryStatus;
}

bool GSM::sendServiceData(const char serviceNumber[]) {
  if (!atUnstructuredSupplementaryServiceData(1, serviceNumber))
    return false;
  return true;
}

bool GSM::sendServiceData(const __FlashStringHelper *serviceNumber) {
  char buffer[strlen_P((const char *)serviceNumber) + 1];
  strcpy_P(buffer, (const char *)serviceNumber);
  return sendServiceData(buffer);
}

void GSM::cancelServiceData(void) {
  atUnstructuredSupplementaryServiceData(2);
}

bool GSM::setOwnNumber(const char ownNumber[]) {
  if(!atSelectPhonebookMemoryStorage("ON")) return false;
  if(!atWritePhonebookEntry(1, ownNumber)) return false;
  return true;
}
