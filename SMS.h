#ifndef SMS_h
#define SMS_h

#include <Arduino.h>
#include "DTE.h"

struct MessageIndication {
  unsigned char mode;
  unsigned char mt;
  unsigned char broadcastMessage;
  unsigned char ds;
  unsigned char buffer;
};

struct Message {
  unsigned char index = 0;
  unsigned char status;
  char address[16] = "";
  unsigned char typeOfAddress = 0;
  char timestamp[21] = "";
  unsigned char firstOctet = 0;
  unsigned int pid = 0;
  unsigned int dataCodingScheme = 0;
  char serviceCenterAddress[16] = "";
  unsigned char typeOfSeviceCenterAddress = 0;
  unsigned char length = 0;
  char data[163] = "";
};

class SMS {
 private:
  DTE *dte;
  bool textMode = false;
  struct MessageIndication messageIndication;
  struct Message message;
  unsigned int mrSend;
  bool showParameter = false;

 public:
  SMS(DTE &dte);

  unsigned char messageStatusIndex(const char status[]);
  unsigned char messageStatusIndex(const __FlashStringHelper *status);

  /**
   * Command AT+CMGD=
   * @param  index   Location
   * @param  delFlag Delete Flag
   *                 0: Delete message on specified index
   *                 1: Delete all read message
   *                 2: Delete all read and sent message
   *                 3: Delete all read, sent, and unsent message
   *                 4: Delete all message
   * @return         true: If command successful, false: Otherwise
   */
  bool atDeleteSMS(unsigned char index, unsigned char delFlag = 0);

  /**
   * Command AT+CMGF?
   * @return  true: If command successful, false: Otherwise
   */
  bool atSelectSMSFormat(void);

  /**
   * Command AT+CMGF=
   * @param  mode SMS Mode
   *              false: PDU Mode, default
   *              true: Text Mode
   * @return      true: If command successful, false: Otherwise
   */
  bool atSelectSMSFormat(bool textMode);

  /**
   * Command AT+CMGL=
   * @param  status List SMS with status
   *                0: "REC UNREAD"
   *                1: "REC READ"
   *                2: "STO UNSENT"
   *                3: "STO SENT"
   *                4: "ALL"
   * @param  mode   Read Mode
   *                0: Normal, change status, default 0
   *                1: Unchange status
   * @return        true: If command successful, false: Otherwise
   */
  bool atListSMS(unsigned char status = 0, unsigned char mode = 0);

  /**
   * Command AT+CMGR=
   * @param  index Location
   * @param  mode  Read Mode
   *               0: Normal, change status from Unread to Read, default
   *               1: Not change status
   * @return       true: If command successful, false: Otherwise
   */
  bool atReadSMS(unsigned char index, unsigned char mode = 0);

  /**
   * Command AT+CMGS=
   * @param  destination Destination Address/Phone Number
   * @param  message     Message
   * @return             true: If command successful, false: Otherwise
   */
  bool atSendSMS(const char destination[], const char message[]);
  bool atSendSMS(const __FlashStringHelper *destination, const char message[]);
  bool atSendSMS(const char destination[], const __FlashStringHelper *message);
  bool atSendSMS(const __FlashStringHelper *destination, const __FlashStringHelper *message);

  bool atNewMessageIndications(void);

  bool atNewMessageIndications(unsigned char mode, unsigned char mt = 255, unsigned char broadcastMessage = 255, unsigned char ds = 255, unsigned char buffer = 255);

  /**
   * Command AT+CSDH?
   * @return  true: If command successful, false: Otherwise
   */
  bool atShowSMSTextModeParameter(void);

  /**
   * Command AT+CSDH=
   * @param  show Show extended parameter in +CMGR
   *              true: Do not show, false: Show
   * @return      true: If command successful, false: Otherwise
   */
  bool atShowSMSTextModeParameter(bool show);

  /**
   * Delete SMS by specified index
   * @param  index Location
   * @return       true: If command successful, false: Otherwise
   */
  bool deleteSMS(unsigned char index) { return atDeleteSMS(index); };

  /**
   * Delete all read SMS
   * @return  true: If command successful, false: Otherwise
   */
  bool deleteSMSAllRead(void) { return atDeleteSMS(0, 1); };

  /**
   * Delete all read and sent SMS
   * @return  true: If command successful, false: Otherwise
   */
  bool deleteSMSAllReadSent(void) { return atDeleteSMS(0, 2); };

  /**
   * Delete all read, sent, and unsent SMS
   * @return  true: If command successful, false: Otherwise
   */
  bool deleteSMSAllReadSentUnsent(void) { return atDeleteSMS(0, 3); };

  /**
   * Delete all SMS
   * @return  true: If command successful, false: Otherwise
   */
  bool deleteSMSAll(void) { return atDeleteSMS(0, 4); };

  /**
   * Is SMS Format Text Mode
   * @return  true: Text Mode, false: PDU Mode
   */
  bool isTextMode(void);

  /**
   * Read SMS. This method is return a Struct that contain
   * Phone Number, and Timestamp
   * @param  index Location
   * @return       Message Struct
   */
  struct Message readSMS(unsigned char index);

  /**
   * Send SMS. If message length longer than 160 character,
   * this method automatically send multiple SMS.
   * @param  destination Destination Address/Phone Number
   * @param  message     Message
   * @return             true: If command successful, false: Otherwise
   */
  bool sendSMS(const char destination[], const char message[]);
  bool sendSMS(const __FlashStringHelper *destination, const char message[]);
  bool sendSMS(const char destination[], const __FlashStringHelper *message);
  bool sendSMS(const __FlashStringHelper *destination, const __FlashStringHelper *message);

  bool newMessageToURC(bool set);

  bool selectSMSFormat(bool mode);

  /**
   * Is show extended parameter for +CGMR
   * @return  true: Show, false: Hide
   */
  bool isShowParameter(void);
};

#endif
