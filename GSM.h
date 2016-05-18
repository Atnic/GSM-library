/*
*
*/
#ifndef GSM_h
#define GSM_h

#include <Arduino.h>
#include <DTE.h>

class GSM
{
private:
  DTE *dte;
  long baudrate;
  struct Operator {
    unsigned char format;
    char oper[16];
    char operNumeric[6];
  } selectedOperator;
  struct PhonebookMemoryStorage {
    char storage[3];
    unsigned char used;
    unsigned char total;
  } phonebookMemoryStorage;
  struct NetworkRegistration {
    unsigned char n;
    unsigned char status;
    char lac[5];
    char ci[5];
  } networkRegistration;
  struct SignalQuality {
    unsigned char rssi;
    unsigned char ber;
  } signalQuality;
  struct Clock {
    char string[21];
    unsigned int year;
    unsigned char month;
    unsigned char day;
    unsigned char hour;
    unsigned char minute;
    unsigned char second;
    unsigned char timezone;
  } clock;
  struct SubscriberNumber {
    char characterSet[9];
    char number[17];
    unsigned char type;
    unsigned char speed;
    unsigned char service;
  } subscriberNumber;
  struct BatteryStatus {
    bool charge;
    unsigned char capacityLevel;
    float voltage;
  } batteryStatus;

public:
  GSM(DTE &dte);

  bool atReIssueLastCommand(void);
  bool atSetCommandEchoMode(bool echo);
  bool atSetFixedLocalRate(void);
  bool atSetFixedLocalRate(long baudrate);

  bool atOperatorSelection(void);
  bool atOperatorSelection(unsigned char mode, unsigned char format = 0, const char oper[] = "");
  bool atOperatorSelection(unsigned char mode, unsigned char format, const __FlashStringHelper oper[]);
  bool atOperatorSelection(unsigned char mode, unsigned char format, unsigned char oper);

  bool atSelectPhonebookMemoryStorage(void);
  bool atSelectPhonebookMemoryStorage(const char storage[]);

  bool atWritePhonebookEntry(unsigned char index);
  bool atWritePhonebookEntry(unsigned char index, const char *phoneNumber);
  bool atWritePhonebookEntry(unsigned char index, const char *phoneNumber, unsigned char type);
  bool atWritePhonebookEntry(unsigned char index, const char *phoneNumber, unsigned char type, const char *text);

  bool atEnterPIN(void);
  bool atEnterPIN(const char *PIN);
  bool atEnterPIN(const char *PIN, const char *newPIN);

  bool atNetworkRegistration(void);
  bool atNetworkRegistration(unsigned char n);

  bool atSignalQualityReport(void);

  bool atSubscriberNumber(void);

  bool atClock(void);
  bool atClock(const char timestamp[]);

  bool atBatteryCharge(void);

  bool atUnstructuredSupplementaryServiceData(void);
  bool atUnstructuredSupplementaryServiceData(unsigned char n);
  bool atUnstructuredSupplementaryServiceData(unsigned char n, const char *str);
  bool atUnstructuredSupplementaryServiceData(unsigned char n, const char *str, unsigned char dcs);

  long getBaudrate(void) { return baudrate; };
  const char *getOperator(void) { return selectedOperator.oper; };
  const char *getOperatorNumeric(void) { return selectedOperator.operNumeric; };
  const char *getPhonebookMemoryStrorage(void) { return phonebookMemoryStorage.storage; };
  unsigned char getNetworkRegistrationStatus(void) { return networkRegistration.status; };
  unsigned char getSignalQualityRSSI(void) { return signalQuality.rssi; };
  const char *getClockString(void) { return clock.string; }
  unsigned char getClockHour(void) { return clock.hour; };
  unsigned char getClockMinute(void) { return clock.minute; };
  const char *getSubscriberNumber(void) { return subscriberNumber.number; };
  bool isBatteryCharging(void) { return batteryStatus.charge; };
  unsigned char getBatteryCapacityLevel(void) { return batteryStatus.capacityLevel; };
  float getBatteryVoltage(void) { return batteryStatus.voltage; };
};

#endif
