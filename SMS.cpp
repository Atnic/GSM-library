#include "SMS.h"
#include "URC.h"

/* SMS Class */
SMS::SMS(DTE &dte) {
  this->dte = &dte;
}

unsigned char SMS::messageStatusIndex(const char status[]) {
  if (strcmp_P(status, (const char *)F("REC UNREAD")) == 0) return 0;
  if (strcmp_P(status, (const char *)F("REC READ")) == 0) return 1;
  if (strcmp_P(status, (const char *)F("STO UNSENT")) == 0) return 2;
  if (strcmp_P(status, (const char *)F("STO SENT")) == 0) return 3;
  return 255;
}

unsigned char SMS::messageStatusIndex(const __FlashStringHelper *status) {
  char buffer[strlen_P((const char *)status) + 1];
  strcpy_P(buffer, (const char *)status);
  return messageStatusIndex(buffer);
}

bool SMS::atDeleteSMS(unsigned char index, unsigned char delFlag) {
  const __FlashStringHelper *command = F("AT+CMGD=%d,%d\r");
  char buffer[13];  // "AT+CMGD=X,X\r"

  sprintf_P(buffer, (const char *)command, index, delFlag);

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(buffer)) return false;
  if (!dte->ATResponseOk()) return false;
  message.index = 0;
  return true;
}

bool SMS::atSelectSMSFormat(void) {
  const __FlashStringHelper *command = F("AT+CMGF?\r");
  const __FlashStringHelper *response = F("+CMGF: ");
  bool textMode;

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(command)) return false;
  if (!dte->ATResponseContain(response)) return false;
  char *str = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
  textMode = (str[0] == '1') ? true : false;
  if (!dte->ATResponseOk()) return false;
  this->textMode = textMode;
  return true;
}

bool SMS::atSelectSMSFormat(bool textMode) {
  const __FlashStringHelper *command = F("AT+CMGF=%d\r");
  char buffer[11];  // "AT+CMGF=X\r"

  sprintf_P(buffer, (const char *)command, textMode ? 1 : 0);

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(buffer)) return false;
  if (!dte->ATResponseOk()) return false;
  this->textMode = textMode;
  return true;
}

// bool SMS::atListSMS(unsigned char status, unsigned char mode) {
// 	char buffer[strlen_P((const char *)F("AT+CMGL=\"XXXXXXXXXX\",X\r\r\n")) + 1];
//   char command[] = "AT+CMGL=";
//   struct Message message;
//
//   if(dte->isPDU())
//     sprintf(buffer, "%s%d,%d\r", command, status, mode);
//   else {
//     char statusText[11];
//     switch (status) {
//       case 0: strcpy_P(statusText, (const char *)F("REC UNREAD")); break;
//       case 1: strcpy_P(statusText, (const char *)F("REC READ")); break;
//       case 2: strcpy_P(statusText, (const char *)F("STO UNSENT")); break;
//       case 3: strcpy_P(statusText, (const char *)F("STO SENT")); break;
//       case 4: strcpy_P(statusText, (const char *)F("ALL")); break;
//     }
//   	sprintf(buffer, "%s\"%s\",%d\r", command, statusText, mode);
//   }
//
// 	dte->clearReceivedBuffer();
// 	dte->ATCommand(buffer);
// 	if(dte->isEcho()) {
// 		if(!dte->ATResponseEqual(strcat(buffer, "\r\n"))) return false;
// 	}
//   if(!dte->ATResponse()) return false;
// 	while(!dte->isResponse(F("OK\r\n"))) {
//     if(dte->isResponseContain(F("+CMGL: "))) {
//     	char *pointer = strstr(dte->getResponse(), "+CMGL: ") + strlen_P((const char *)F("+CMGL: "));
//     	char *str = strtok(pointer, ",\r\n");
//       for (unsigned char i = 0; i < 7 && str != NULL; i++) {
//         if(i == 0) message.index = str[0] - '0';
//         if(i == 1) {
//           str++; str[strlen(str)-1] = '\0';
//           message.status = messageStatusIndex(str);
//           if(message.status == 0 || message.status == 2)
//             if(mode == 0)
//               message.status++;
//         }
//         if(i == 2) {
//           if(str[0] == '"') {
//             str++; str[strlen(str)-1] = '\0';
//             strcpy(message.address, str);
//           }
//           else
//             message.firstOctet = atoi(str);
//         }
//         if(i == 3) {
//           if(message.firstOctet != 0)
//             message.mr = atoi(str);
//         }
//         if(i == 4) {
//           if(str[0] == '"') {
//             str++; str[strlen(str)-1] = '\0';
//             strcpy(message.timestamp, str);
//           }
//         }
//         str = strtok(NULL, ",\r\n");
//       }
//       strcpy(message.data, "");
//       if(strlen(message.address) > 0) {
//         if(!dte->ATResponse()) return false;
//         while(!dte->isResponse(F("OK\r\n")) && !dte->isResponseContain(F("+CMGL: "))) {
//           if(dte->getResponse()[0] == '+')
//             urc.unsolicitedResultCode(dte->getResponse());
//           else strcat(message.data, dte->getResponse());
//           if(!dte->ATResponse()) return false;
//         }
//       }
//       this->message[message.index - 1] = message;
//     }
//     else if(!dte->ATResponse()) return false;
//   }
//   return true;
// }

bool SMS::atReadSMS(unsigned char index, unsigned char mode) {
  const __FlashStringHelper *command = F("AT+CMGR=%d,%d\r");
  const __FlashStringHelper *response = F("+CMGR: ");
  char buffer[13];  // "AT+CMGR=X,X\r"
  struct Message message;

  sprintf_P(buffer, (const char *)command, index, mode);

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(buffer)) return false;
  if (dte->ATResponse()) {
    if (dte->isResponseOk()) {
      this->message = message;
      return true;
    }
    if (!dte->isResponseContain(response)) return false;
  } else
    return false;
  message.index = index;
  if (!dte->ATResponse(message.data, 163)) return false;
  char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
  char *str = strtok(pointer, "\",");
  for (size_t i = 0; str != NULL; i++) {
    if (i == 0) {
      if (isdigit(str[0]))
        message.status = str[0] - '0';
      else {
        message.status = messageStatusIndex(str);
      }
      if (message.status == 0 && mode == 0) message.status++;
    }
    if (i == 1) {
      if (str[0] == '\"') {
        strncpy(message.address, str + 1, strlen(str + 1) - 1);
        message.address[strlen(str + 1) - 1] = '\0';
      } else
        message.firstOctet = atoi(str);
    }
    if ((message.firstOctet & 0x03) == 0x00 ||
        (message.firstOctet & 0x03) == 0x02) {
      if (i == 3) {
        if (strlen(str) == 20) {
          strcpy(message.timestamp, str);
          str = strtok(NULL, ",");
        }
        message.typeOfAddress = atoi(str);
      }
      if (i == 4) message.firstOctet = atoi(str);
      if (i == 5) message.pid = atoi(str);
      if (i == 6) message.dataCodingScheme = atoi(str);
      if (i == 7) {
        if ((message.firstOctet & 0x03) == 0x02) str = strtok(NULL, ",");
        strncpy(message.serviceCenterAddress, str + 1, strlen(str + 1) - 1);
        message.serviceCenterAddress[strlen(str + 1) - 1] = '\0';
      }
      if (i == 8) message.typeOfSeviceCenterAddress = atoi(str);
      if (i == 9) message.length = atoi(str);
      if (i == 2)
        str = strtok(NULL, "\"");
      else
        str = strtok(NULL, ",");
    } else {
      if (i == 2) {
        // if(strlen(message.address) == 0) message.mr = atoi(str);
      }
      str = strtok(NULL, ",");
    }
  }
  if (!dte->ATResponseOk()) return false;
  this->message = message;

  return true;
}

bool SMS::atSendSMS(const char destination[], const char message[]) {
  const __FlashStringHelper *command = F("AT+CMGS=\"%s\"\r");
  const __FlashStringHelper *response = F("+CMGS: ");
  char buffer[12 + strlen(destination)];  // "AT+CMGS=\"{destination}\"\r"

  sprintf_P(buffer, (const char *)command, destination);

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(buffer)) return false;
  if (!dte->ATResponseContain("> ")) return false;
  unsigned char length = 0;
  for (size_t i = 0; i < strlen(message) && length < 160; i++) {
    if (message[i] != '\r') {
      dte->write(message[i]);
      length++;
    }
  }
  dte->write('\x1A');
  if (dte->isEcho()) {
    dte->flush();
    dte->ATResponse(10);
  }
  if (!dte->ATResponseContain(response, 30000)) return false;
  if (!dte->ATResponseOk()) return false;
  return true;
}

bool SMS::atSendSMS(const __FlashStringHelper *destination, const char message[]) {
  char buffer[strlen_P((const char *)destination) + 1];
  strcpy_P(buffer, (const char *)destination);
  return atSendSMS(buffer, message);
}

bool SMS::atSendSMS(const char destination[], const __FlashStringHelper *message) {
  char buffer[strlen_P((const char *)message) + 1];
  strcpy_P(buffer, (const char *)message);
  return atSendSMS(destination, buffer);
}

bool SMS::atSendSMS(const __FlashStringHelper *destination, const __FlashStringHelper *message) {
  char buffer[strlen_P((const char *)message) + 1];
  strcpy_P(buffer, (const char *)message);
  return atSendSMS(destination, buffer);
}

bool SMS::atNewMessageIndications(void) {
  const __FlashStringHelper *command = F("AT+CNMI?\r");
  const __FlashStringHelper *response = F("+CNMI: ");
  struct MessageIndication messageIndication;

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(command)) return false;
  if (!dte->ATResponseContain(response)) return false;
  char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
  char *str = strtok(pointer, ",");
  for (size_t i = 0; str != NULL; i++) {
    if (i == 0) messageIndication.mode = atoi(str);
    if (i == 1) messageIndication.mt = atoi(str);
    if (i == 2) messageIndication.broadcastMessage = atoi(str);
    if (i == 3) messageIndication.ds = atoi(str);
    if (i == 4) messageIndication.buffer = atoi(str);
    str = strtok(NULL, ",");
  }
  showParameter = (str[0] == '1') ? true : false;
  if (!dte->ATResponseOk()) return false;
  this->messageIndication = messageIndication;
  return true;
}

bool SMS::atNewMessageIndications(unsigned char mode, unsigned char mt, unsigned char broadcastMessage, unsigned char ds, unsigned char buffer) {
  const __FlashStringHelper *command;
  if (mt == 255)
    command = F("AT+CNMI=%d\r");
  else if (broadcastMessage == 255)
    command = F("AT+CNMI=%d,%d\r");
  else if (ds == 255)
    command = F("AT+CNMI=%d,%d,%d\r");
  else if (buffer == 255)
    command = F("AT+CNMI=%d,%d,%d,%d\r");
  else
    command = F("AT+CNMI=%d,%d,%d,%d,%d\r");

  char bufferCommand[19];  // "AT+CNMI=X,X,X,X,X\r"
  if (mt == 255)
    sprintf_P(bufferCommand, (const char *)command, mode);
  else if (broadcastMessage == 255)
    sprintf_P(bufferCommand, (const char *)command, mode, mt);
  else if (ds == 255)
    sprintf_P(bufferCommand, (const char *)command, mode, mt, broadcastMessage);
  else if (buffer == 255)
    sprintf_P(bufferCommand, (const char *)command, mode, mt, broadcastMessage, ds);
  else
    sprintf_P(bufferCommand, (const char *)command, mode, mt, broadcastMessage, ds, buffer);

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(bufferCommand)) return false;
  if (!dte->ATResponseOk()) return false;
  return atNewMessageIndications();
}

bool SMS::atShowSMSTextModeParameter(void) {
  const __FlashStringHelper *command = F("AT+CSDH?\r");
  const __FlashStringHelper *response = F("+CSDH: ");
  bool showParameter;

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(command)) return false;
  if (!dte->ATResponseContain(response)) return false;
  char *str = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
  showParameter = (str[0] == '1') ? true : false;
  if (!dte->ATResponseOk()) return false;
  this->showParameter = showParameter;
  return true;
}

bool SMS::atShowSMSTextModeParameter(bool show) {
  const __FlashStringHelper *command = F("AT+CSDH=%d\r");
  char buffer[11];  // "AT+CMGF=X\r"

  sprintf_P(buffer, (const char *)command, show);

  dte->clearReceivedBuffer();
  if (!dte->ATCommand(buffer)) return false;
  if (!dte->ATResponseOk()) return false;
  showParameter = show;
  return true;
}

bool SMS::isTextMode(void) {
  atSelectSMSFormat();
  return textMode;
}

struct Message SMS::readSMS(unsigned char index) {
  struct Message message;

  if (!isTextMode()) {
    if (!atSelectSMSFormat(true)) return message;
  }
  if (!isShowParameter()) {
    if (!atShowSMSTextModeParameter(true)) {
      this->message = message;
      return this->message;
    }
  }
  if (this->message.index != index) {
    if (!atReadSMS(index)) {
      this->message = message;
      return this->message;
    }
  }
  return this->message;
}

bool SMS::sendSMS(const char destination[], const char message[]) {
  bool success = false;

  if (!isTextMode()) {
    if (!atSelectSMSFormat(true)) return false;
  }
  unsigned int partial = (strlen(message) / 160) + 1;
  for (size_t i = 0; i < partial; i++) {
    char messageBuffer[161] = "";
    strncpy(messageBuffer, message + (i * 160), 160);
    messageBuffer[160] = '\0';
    if (i < partial - 1) {
    } else {
    }
    success = atSendSMS(destination, messageBuffer);
  }

  return success;
}

bool SMS::sendSMS(const __FlashStringHelper *destination, const char message[]) {
  char buffer[strlen_P((const char *)destination) + 1];
  strcpy_P(buffer, (const char *)destination);
  return sendSMS(buffer, message);
}

bool SMS::sendSMS(const char destination[], const __FlashStringHelper *message) {
  char buffer[strlen_P((const char *)message) + 1];
  strcpy_P(buffer, (const char *)message);
  return sendSMS(destination, buffer);
}

bool SMS::sendSMS(const __FlashStringHelper *destination, const __FlashStringHelper *message) {
  char buffer[strlen_P((const char *)message) + 1];
  strcpy_P(buffer, (const char *)message);
  return sendSMS(destination, buffer);
}

bool SMS::selectSMSFormat(bool mode) {
  if (isTextMode() == mode) {
    if(!atSelectSMSFormat(mode)) return false;
  }

  return true;
}

bool SMS::newMessageToURC(bool set) {
  if (!isTextMode()) {
    if (!atSelectSMSFormat(true)) return false;
  }
  if (!isShowParameter()) {
    if (!atShowSMSTextModeParameter(true)) return false;
  }
  atNewMessageIndications();
  atNewMessageIndications(messageIndication.mode, set ? 2 : 1);
  return showParameter;
}

bool SMS::isShowParameter(void) {
  atShowSMSTextModeParameter();
  return showParameter;
}
