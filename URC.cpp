#include "URC.h"

URC Urc;

URC::URC() {
  newMessage.message = NULL;
}

bool URC::unsolicitedResultCode(const char urcResponse[]) {
  const __FlashStringHelper *urcCallReady = F("Call Ready");
  const __FlashStringHelper *urcHttpAction = F("+HTTPACTION:");
  const __FlashStringHelper *urcEnterPin = F("+CPIN: ");
  const __FlashStringHelper *urcNewMessageIndication = F("+CMTI: ");
  const __FlashStringHelper *urcNewMessage = F("+CMT: ");
  const __FlashStringHelper *urcServiceDataIndication = F("+CUSD: ");
  const __FlashStringHelper *urcGetLocalTimestamp = F("*PSUTTZ: ");

  char *pointer;
  char urc[strlen(urcResponse) + 1];

  strcpy(urc, urcResponse);
  if ((pointer = strstr_P(urc, (const char *)urcCallReady)) != NULL) {
    callReady.updated = true;
    return true;
  } else if ((pointer = strstr_P(urc, (const char *)urcHttpAction)) != NULL) {
    pointer += strlen_P((const char *)urcHttpAction);
    char *str = strtok(pointer, ",");
    unsigned char i = 0;
    for (i = 0; i < 3 && str != NULL; i++) {
      if (i == 0) httpAction.method = str[0] - '0';
      if (i == 1) httpAction.statusCode = atoi(str);
      if (i == 2) httpAction.dataLength = atoi(str);
      str = strtok(NULL, ",");
    }
    if (i >= 3) httpAction.updated = true;
    return true;
  } else if ((pointer = strstr_P(urc, (const char *)urcEnterPin)) != NULL) {
    pointer += strlen_P((const char *)urcEnterPin);
    char *str = strtok(pointer, "\"");
    strcpy(enterPin.code, str);
    enterPin.updated = true;
    return true;
  } else if ((pointer = strstr_P(urc, (const char *)urcGetLocalTimestamp)) != NULL) {
    pointer += strlen_P((const char *)urcGetLocalTimestamp);
    char *str = strtok(pointer, ",\" +");
    for (unsigned char i = 0; i < 8 && str != NULL; i++) {
      if (i == 0) psuttz.year = atoi(str);
      if (i == 1) psuttz.month = atoi(str);
      if (i == 2) psuttz.day = atoi(str);
      if (i == 3) psuttz.hour = atoi(str);
      if (i == 4) psuttz.minute = atoi(str);
      if (i == 5) psuttz.second = atoi(str);
      if (i == 6) psuttz.timezone = atoi(str) / 4;
      str = strtok(NULL, ",\" +");
    }
    psuttz.updated = true;
    return true;
  } else if ((pointer = strstr_P(urc, (const char *)urcNewMessageIndication)) != NULL) {
    pointer += strlen_P((const char *)urcNewMessageIndication);
    char *str = strtok(pointer, "\",");
    strcpy(newMessageIndication.mem, str);
    str = strtok(NULL, "\",");
    newMessageIndication.index = atoi(str);
    newMessageIndication.updated = true;
    return true;
  } else if ((pointer = strstr_P(urc, (const char *)urcNewMessage)) != NULL) {
    if (newMessage.message == NULL) return false;
    strcpy(newMessage.message->data, urc);
    newMessage.waiting = true;
    return true;
  } else if ((pointer = strstr_P(urc, (const char *)urcServiceDataIndication)) != NULL) {
    pointer += strlen_P((const char *)urcServiceDataIndication);
    char *str = strtok(pointer, ",");
    serviceDataIndication.n = atoi(str);
    str = strtok(NULL, "\"");
    strcpy(serviceDataIndication.str, str);
    str = strtok(NULL, "\",");
    serviceDataIndication.dcs = atoi(str);
    serviceDataIndication.updated = true;
    return true;
  } else if (newMessage.waiting) {
    if (newMessage.message == NULL) return false;
    pointer = strstr_P(newMessage.message->data, (const char *)urcNewMessage) + strlen_P((const char *)urcNewMessage);
    char *str = strtok(pointer, ",");
    for (size_t i = 0; str != NULL; i++) {
      if (i == 0) {
        if (str[0] == '\"') {
          strncpy(newMessage.message->address, str + 1, strlen(str + 1) - 1);
          newMessage.message->address[strlen(str + 1) - 1] = '\0';
        } else
          newMessage.message->firstOctet = atoi(str);
      }
      if ((newMessage.message->firstOctet & 0x03) == 0x00 ||
          (newMessage.message->firstOctet & 0x03) == 0x02) {
        if (i == 2) {
          if (strlen(str) == 20) {
            strcpy(newMessage.message->timestamp, str);
            str = strtok(NULL, ",");
          }
          newMessage.message->typeOfAddress = atoi(str);
        }
        if (i == 3) newMessage.message->firstOctet = atoi(str);
        if (i == 4) newMessage.message->pid = atoi(str);
        if (i == 5) newMessage.message->dataCodingScheme = atoi(str);
        if (i == 6) {
          if ((newMessage.message->firstOctet & 0x03) == 0x02) str = strtok(NULL, ",");
          strncpy(newMessage.message->serviceCenterAddress, str + 1, strlen(str + 1) - 1);
          newMessage.message->serviceCenterAddress[strlen(str + 1) - 1] = '\0';
        }
        if (i == 7) newMessage.message->typeOfSeviceCenterAddress = atoi(str);
        if (i == 8) newMessage.message->length = atoi(str);
        if (i == 1)
          str = strtok(NULL, "\"");
        else
          str = strtok(NULL, ",");
      } else {
        if (i == 1) {
          // if(strlen(newMessage.message->address) == 0) newMessage.message->mr = atoi(str);
        }
        str = strtok(NULL, ",");
      }
    }
    strcpy(newMessage.message->data, urc);
    newMessage.waiting = false;
    newMessage.updated = true;
  }
  return false;
}

bool URC::unsolicitedResultCode(const __FlashStringHelper *urc) {
  char buffer[strlen_P((const char *)urc) + 1];
  strcpy_P(buffer, (const char *)urc);
  return unsolicitedResultCode(buffer);
}

void URC::resetUnsolicitedResultCode(void) {
  callReady.updated = false;
  enterPin.updated = false;
  httpAction.updated = false;
  psuttz.updated = false;
  newMessageIndication.updated = false;
  newMessage.updated = false;
  serviceDataIndication.updated = false;
}
