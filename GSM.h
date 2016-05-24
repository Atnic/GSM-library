/*
*
*/
#ifndef GSM_h
#define GSM_h

#include <Arduino.h>
#include <DTE.h>

struct Operator {
  unsigned char mode = 0;
  unsigned char format = 0;
  char oper[16] = "";
  char operNumeric[6] = "";
};

struct PhonebookMemoryStorage {
  char storage[3] = "";
  unsigned char used;
  unsigned char total;
};

struct NetworkRegistration {
  unsigned char n = 0;
  unsigned char status = 4;
  char lac[5] = "";
  char ci[5] = "";
};

struct SignalQuality {
  unsigned char rssi = 0;
  unsigned char ber = 0;
};

struct Clock {
  char timestamp[21] = "";
  unsigned int year;
  unsigned char month;
  unsigned char day;
  unsigned char hour;
  unsigned char minute;
  unsigned char second;
  unsigned char timezone;
};

struct SubscriberNumber {
  char characterSet[9] = "";
  char number[17] = "";
  unsigned char type;
  unsigned char speed;
  unsigned char service;
};

struct BatteryStatus {
  bool charge = false;
  unsigned char capacityLevel = 0;
  float voltage = 0.0f;
};

class GSM
{
private:
  DTE *dte;
  long baudrate = -1;
  struct Operator selectedOperator;
  struct PhonebookMemoryStorage phonebookMemoryStorage;
  char pinStatus[11];
  struct NetworkRegistration networkRegistration;
  struct SignalQuality signalQuality;
  struct Clock clock;
  struct SubscriberNumber subscriberNumber;
  struct BatteryStatus batteryStatus;

  bool atReIssueLastCommand(void);
  bool atSetCommandEchoMode(bool echo);
  bool atSetFixedLocalRate(void);
  bool atSetFixedLocalRate(long baudrate);

  bool atOperatorSelection(void);
  bool atOperatorSelection(unsigned char mode, unsigned char format = 0, const char oper[] = "");
  bool atOperatorSelection(unsigned char mode, unsigned char format, const __FlashStringHelper *oper);

  bool atSelectPhonebookMemoryStorage(void);
  bool atSelectPhonebookMemoryStorage(const char storage[]);

  bool atWritePhonebookEntry(unsigned char index);
  bool atWritePhonebookEntry(unsigned char index, const char phoneNumber[]);
  bool atWritePhonebookEntry(unsigned char index, const char phoneNumber[], unsigned char type);
  bool atWritePhonebookEntry(unsigned char index, const char phoneNumber[], unsigned char type, const char text[]);

  bool atEnterPIN(void);
  bool atEnterPIN(const char pin[]);
  bool atEnterPIN(const char pin[], const char *newPin);

  bool atNetworkRegistration(void);
  bool atNetworkRegistration(unsigned char n);

  bool atSignalQualityReport(void);

  bool atSubscriberNumber(void);

  bool atClock(void);
  bool atClock(const char timestamp[]);

  bool atBatteryCharge(void);

  bool atUnstructuredSupplementaryServiceData(void);
  bool atUnstructuredSupplementaryServiceData(unsigned char n);
  bool atUnstructuredSupplementaryServiceData(unsigned char n, const char str[]);
  bool atUnstructuredSupplementaryServiceData(unsigned char n, const char str[], unsigned char dcs);

public:
  GSM(DTE &dte);

  long getBaudrate(void);
  struct Operator getOperator(unsigned char format = 0);
  struct PhonebookMemoryStorage getPhonebookMemoryStrorage(void);
  const char *getPinStatus(void);
  struct NetworkRegistration getNetworkRegistration(void);
  struct SignalQuality getSignalQuality(void);
  struct Clock getClock(void);
  struct SubscriberNumber getSubscriberNumber(void);
  struct BatteryStatus getBatteryStatus(void);
};

#endif
