#ifndef GSM_h
#define GSM_h

#include <Arduino.h>
#include "DTE.h"

struct Operator {
  unsigned char mode;
  unsigned char format;
  char oper[16];
  char operNumeric[6];
};

struct ServiceData {
  unsigned char n;
};

struct PhonebookMemoryStorage {
  char storage[3];
  unsigned char used;
  unsigned char total;
};

struct NetworkRegistration {
  unsigned char n;
  unsigned char status;
  char lac[5];
  char ci[5];
};

struct SignalQuality {
  unsigned char rssi;
  unsigned char ber;
};

struct Clock {
  char timestamp[21];
  unsigned int year;
  unsigned char month;
  unsigned char day;
  unsigned char hour;
  unsigned char minute;
  unsigned char second;
  unsigned char timezone;
};

struct SubscriberNumber {
  char characterSet[9];
  char number[17];
  unsigned char type;
  unsigned char speed;
  unsigned char service;
};

struct BatteryStatus {
  bool charge;
  unsigned char capacityLevel;
  float voltage;
};

class GSM {
 private:
  DTE *dte;
  struct Operator selectedOperator;
  struct ServiceData serviceData;
  struct PhonebookMemoryStorage phonebookMemoryStorage;
  char pinStatus[11];
  struct NetworkRegistration networkRegistration;
  struct SignalQuality signalQuality;
  struct Clock clock;
  struct SubscriberNumber subscriberNumber;
  struct BatteryStatus batteryStatus;

 public:
  GSM(DTE &dte);

  /**
   * Command AT+COPS?
   * @return  true: If command successful, false: Otherwise
   */
  bool atOperatorSelection(void);

  /**
   * Command AT+COPS=
   * @param  mode   Selection mode.
   *                0: Automatic mode
   *                1: Manual mode, oper should be present
   *                3: Setting format
   *                4: Manual and Automatic, oper should be present, if fail, then
   *                	 automatic.
   * @param  format Operator format
   *                0: Long format alphanumeric
   *                1: Short format alphanumeric
   *                2: Numeric format
   * @param  oper   Operator
   * @return        true: If command successful, false: Otherwise
   */
  bool atOperatorSelection(unsigned char mode, unsigned char format = 0, const char oper[] = "");
  bool atOperatorSelection(unsigned char mode, unsigned char format, const __FlashStringHelper *oper);

  /**
   * Command AT+CPBS?
   * @return  true: If command successful, false: Otherwise
   */
  bool atSelectPhonebookMemoryStorage(void);

  /**
   * Command AT+CPBS=
   * @param  storage Storage Name
   * @return         true: If command successful, false: Otherwise
   */
  bool atSelectPhonebookMemoryStorage(const char storage[]);
  bool atSelectPhonebookMemoryStorage(const __FlashStringHelper *storage);

  /**
   * Command AT+CPBW=
   * @param  index Location number
   * @return       true: If command successful, false: Otherwise
   */
  bool atWritePhonebookEntry(unsigned char index);

  /**
   * Command AT+CPBW=
   * @param  index       Location Number
   * @param  phoneNumber Phone Number
   * @return             true: If command successful, false: Otherwise
   */
  bool atWritePhonebookEntry(unsigned char index, const char phoneNumber[]);
  bool atWritePhonebookEntry(unsigned char index, const __FlashStringHelper *phoneNumber);

  /**
   * Command AT+CPBW=
   * @param  index       Location Number
   * @param  phoneNumber Phone Number
   * @param  type        Type of Number:
   *                     129: National Number type
   *                     161: National Number type
   *                     145: International Number type
   *                     177: Network Specific Number
   * @return             true: If command successful, false: Otherwise
   */
  bool atWritePhonebookEntry(unsigned char index, const char phoneNumber[], unsigned char type);
  bool atWritePhonebookEntry(unsigned char index, const __FlashStringHelper *phoneNumber, unsigned char type);

  /**
   * Command AT+CPBW=
   * @param  index       Location Number
   * @param  phoneNumber Phone Number
   * @param  type        Type of Number:
   *                     129: National Number type
   *                     161: National Number type
   *                     145: International Number type
   *                     177: Network Specific Number
   * @param  text        Text associated with number
   * @return             true: If command successful, false: Otherwise
   */
  bool atWritePhonebookEntry(unsigned char index, const char phoneNumber[], unsigned char type, const char text[]);
  bool atWritePhonebookEntry(unsigned char index, const __FlashStringHelper *phoneNumber, unsigned char type, const char text[]);
  bool atWritePhonebookEntry(unsigned char index, const char phoneNumber[], unsigned char type, const __FlashStringHelper *text);
  bool atWritePhonebookEntry(unsigned char index, const __FlashStringHelper *phoneNumber, unsigned char type, const __FlashStringHelper *text);

  /**
   * Command AT+CPIN?
   * @return  true: If command successful, false: Otherwise
   */
  bool atEnterPIN(void);

  /**
   * Command AT+CPIN=
   * @param  pin SIM Pin
   * @return     true: If command successful, false: Otherwise
   */
  bool atEnterPIN(const char pin[]);
  bool atEnterPIN(const __FlashStringHelper *pin);

  /**
   * Command AT+CPIN=
   * @param  pin    SIM Pin
   * @param  newPin New SIM Pin
   * @return        true: If command successful, false: Otherwise
   */
  bool atEnterPIN(const char pin[], const char newPin[]);
  bool atEnterPIN(const __FlashStringHelper *pin, const char newPin[]);
  bool atEnterPIN(const char pin[], const __FlashStringHelper *newPin);
  bool atEnterPIN(const __FlashStringHelper *pin, const __FlashStringHelper *newPin);

  /**
   * Command AT+CREG?
   * @return  true: If command successful, false: Otherwise
   */
  bool atNetworkRegistration(void);

  /**
   * Command AT+CREG=
   * @param  n URC Control
   *           0: Disable URC
   *           1: Enable URC
   *           2: Enable URC, with location information
   * @return   true: If command successful, false: Otherwise
   */
  bool atNetworkRegistration(unsigned char n);

  /**
   * Command AT+CSQ
   * @return  true: If command successful, false: Otherwise
   */
  bool atSignalQualityReport(void);

  /**
   * Command AT+CNUM
   * @return  true: If command successful, false: Otherwise
   */
  bool atSubscriberNumber(void);

  /**
   * Command AT+CCLK?
   * @return  true: If command successful, false: Otherwise
   */
  bool atClock(void);

  /**
   * Command AT+CCLK=
   * @param  timestamp Timestamp in format
   * @return           true: If command successful, false: Otherwise
   */
  bool atClock(const char timestamp[]);
  bool atClock(const __FlashStringHelper *timestamp);

  /**
   * Command AT+CBC
   * @return  true: If command successful, false: Otherwise
   */
  bool atBatteryCharge(void);

  /**
   * Command AT+CUSD?
   * @return  true: If command successful, false: Otherwise
   */
  bool atUnstructuredSupplementaryServiceData(void);

  /**
   * Command AT+CUSD=
   * @param  n Control USSD, Code result presentation
   *           0: Disable
   *           1: Enable
   *           2: Cancel Session
   * @return   true: If command successful, false: Otherwise
   */
  bool atUnstructuredSupplementaryServiceData(unsigned char n);

  /**
   * Command AT+CUSD=
   * @param  n   Control USSD, Code result presentation
   *             0: Disable
   *             1: Enable
   * @param  str USSD Code, ex: *123#
   * @return     true: If command successful, false: Otherwise
   */
  bool atUnstructuredSupplementaryServiceData(unsigned char n, const char str[]);
  bool atUnstructuredSupplementaryServiceData(unsigned char n, const __FlashStringHelper *str);

  /**
   * Command AT+CUSD=
   * @param  n   Control USSD, Code result presentation
   *             0: Disable
   *             1: Enable
   * @param  str USSD Code, ex: *123#
   * @param  dcs Cell Broadcast Data Coding Scheme
   * @return     true: If command successful, false: Otherwise
   */
  bool atUnstructuredSupplementaryServiceData(unsigned char n, const char str[], unsigned char dcs);
  bool atUnstructuredSupplementaryServiceData(unsigned char n, const __FlashStringHelper *str, unsigned char dcs);

  /**
   * Get current Operator, this method also
   * automatically fetch Operator in Numeric Format
   * @param  format Format operator name, default: 0
   *                0: Long name format
   *                1: Short name format
   * @return        Operator Struct
   */
  struct Operator getOperator(unsigned char format = 0);

  /**
   * Get current Phonebook Memory Storage
   * @return  PhonebookMemoryStorage Struct
   */
  struct PhonebookMemoryStorage getPhonebookMemoryStrorage(void);

  /**
   * Get current Pin Status
   * @return  Pin Status in char array
   */
  const char *getPinStatus(void);

  /**
   * Get current Network Registration
   * @return  NetworkRegistration Struct
   */
  struct NetworkRegistration getNetworkRegistration(void);

  /**
   * Get current Signal Quality
   * @return  SignalQuality Struct
   */
  struct SignalQuality getSignalQuality(void);

  /**
   * Get current Timestatmp
   * @return  Clock Struct
   */
  struct Clock getClock(void);

  /**
   * Get Subscriber/Owner Number
   * @return  SubscriberNumber Struct
   */
  struct SubscriberNumber getSubscriberNumber(void);

  /**
   * Get current Battery Status
   * @return  BatteryStatus Struct
   */
  struct BatteryStatus getBatteryStatus(void);

  /**
   * Send USSD data
   * @param  serviceNumber String to access USSD, ex: *123#
   * @return               true: If command successful, false: Otherwise
   */
  bool sendServiceData(const char serviceNumber[]);
  bool sendServiceData(const __FlashStringHelper *serviceNumber);

  /**
   * Cancel/close USSD session
   */
  void cancelServiceData(void);

  /**
   * Set Own Number
   */
  bool setOwnNumber(const char ownNumber[]);
};

#endif
