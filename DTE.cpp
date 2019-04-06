#include "DTE.h"
#include "URC.h"

void DTE::debugPrint(const char message[], bool returnChar) {
  if (debug) {
    Serial.print(message);
    if (returnChar) Serial.println();
  }
}

void DTE::debugPrint(const __FlashStringHelper *message, bool returnChar) {
  char buffer[strlen_P((const char *)message) + 1];
  strcpy_P(buffer, (const char *)message);
  debugPrint(buffer, returnChar);
}

/* DTE Class */
DTE::DTE(HardwareSerial &hardwareSerial, int powerPin, bool debug) {
  this->hardwareSerial = &hardwareSerial;
  this->softwareSerial = NULL;
  this->powerPin = powerPin;
  this->debug = debug;
  this->response[0] = '\0';
  this->echo = true;
  this->flowControl = (struct FlowControl){0, true, 0, true};
  this->baudrate = -1;
  this->powerDown = true;

  pinMode(this->powerPin, OUTPUT);
  digitalWrite(this->powerPin, LOW);
}

DTE::DTE(SoftwareSerial &softwareSerial, int powerPin, bool debug) {
  this->hardwareSerial = NULL;
  this->softwareSerial = &softwareSerial;
  this->powerPin = powerPin;
  this->debug = debug;
  this->response[0] = '\0';
  this->echo = true;
  this->flowControl = (struct FlowControl){0, true, 0, true};
  this->baudrate = -1;
  this->powerDown = true;

  pinMode(this->powerPin, OUTPUT);
  digitalWrite(this->powerPin, LOW);
}

bool DTE::atReIssueLastCommand(void) {
  const __FlashStringHelper *command = F("A/\r");

  clearReceivedBuffer();
  if (!ATCommand(command)) return false;
  if (!ATResponseOk()) return false;
  return true;
}

bool DTE::atSetCommandEchoMode(bool echo) {
  const __FlashStringHelper *command = F("ATE%d&W\r");
  char buffer[8];  // "ATEX&W\r"

  sprintf_P(buffer, (const char *)command, echo ? 1 : 0);

  clearReceivedBuffer();
  if (!ATCommand(buffer)) return false;
  if (!ATResponseOk()) return false;
  this->echo = echo;
  return true;
}

bool DTE::atRequestProductSerialNumberIdentification(void) {
  const __FlashStringHelper *command = F("AT+GSN\r");
  char productSerialNumberIdentification[17];

  clearReceivedBuffer();
  if (!ATCommand(command)) return false;
  if (!ATResponse()) return false;
  while (!isResponseOk() && !isdigit(*getResponse())) {
    if (!ATResponse()) return false;
  }
  if (!isResponseOk()) {
    strcpy(productSerialNumberIdentification, getResponse());
  }
  if (!ATResponseOk()) return false;
  strcpy(this->productSerialNumberIdentification, productSerialNumberIdentification);
  return true;
}

bool DTE::atSetLocalDataFlowControl(void) {
  const __FlashStringHelper *command = F("AT+IFC?\r");
  const __FlashStringHelper *response = F("+IFC: ");
  struct FlowControl flowControl;

  flowControl = this->flowControl;
  clearReceivedBuffer();
  if (!ATCommand(command)) return false;
  if (!ATResponseContain(response)) return false;
  char *pointer = strstr_P(getResponse(), (const char *)response) + strlen_P((const char *)response);
  char *str = strtok(pointer, ",");
  for (size_t i = 0; i < 2 && str != NULL; i++) {
    if (i == 0) flowControl.dce = str[0] - '0';
    if (i == 1) flowControl.dte = str[0] - '0';
    str = strtok(NULL, ",");
  }
  if (!ATResponseOk()) return false;
  this->flowControl = flowControl;
  return true;
}

bool DTE::atSetLocalDataFlowControl(unsigned char dce, unsigned char dte) {
  char buffer[16];  // "AT+IFC=X,X;&W\r"

  if (dte == 1)
    sprintf_P(buffer, (const char *) F("AT+IFC=%d,%d;&W\r"), dce, dte);
  else
    sprintf_P(buffer, (const char *) F("AT+IFC=%d;&W\r"), dce);

  clearReceivedBuffer();
  if (!ATCommand(buffer)) return false;
  if (!ATResponseOk()) return false;
  flowControl.dce = dce;
  flowControl.dte = dte;
  return true;
}

bool DTE::atSetFixedLocalRate(void) {
  const __FlashStringHelper *command = F("AT+IPR?\r");
  const __FlashStringHelper *response = F("+IPR: ");
  long baudrate = 0;

  clearReceivedBuffer();
  if (!ATCommand(command)) return false;
  if (!ATResponseContain(response)) return false;
  char *str = strstr_P(getResponse(), (const char *)response) + strlen_P((const char *)response);
  baudrate = atoi(str);
  if (!ATResponseOk()) return false;
  this->baudrate = baudrate;
  return true;
}

bool DTE::atSetFixedLocalRate(long baudrate) {
  const __FlashStringHelper *command = F("AT+IPR=%ld\r");
  char buffer[15];  // "AT+IPR=XXXXXX\r"

  sprintf_P(buffer, (const char *)command, baudrate);

  clearReceivedBuffer();
  if (!ATCommand(buffer)) return false;
  if (!ATResponseOk()) return false;
  this->baudrate = baudrate;
  return true;
}

int DTE::available(void) {
  if (hardwareSerial) return hardwareSerial->available();
  if (softwareSerial) return softwareSerial->available();
  return -1;
}

bool DTE::isListening(void) {
  if (softwareSerial) return softwareSerial->isListening();
  return false;
}

bool DTE::listen(void) {
  if (softwareSerial) return softwareSerial->listen();
  return false;
}

void DTE::flush(void) {
  if (hardwareSerial) hardwareSerial->flush();
  if (softwareSerial) softwareSerial->flush();
}

size_t DTE::write(const char c) {
  if (hardwareSerial) return hardwareSerial->write(c);
  if (softwareSerial) return softwareSerial->write(c);
  return 0;
}

size_t DTE::write(const char str[]) {
  if (hardwareSerial) return hardwareSerial->write(str);
  if (softwareSerial) return softwareSerial->write(str);
  return 0;
}

size_t DTE::write(const __FlashStringHelper *str) {
  char buffer[strlen_P((const char *)str) + 1];
  strcpy_P(buffer, (const char *)str);
  return write(buffer);
}

size_t DTE::readBytes(char buffer[], size_t length) {
  if (hardwareSerial) return hardwareSerial->readBytes(buffer, length);
  if (softwareSerial) return softwareSerial->readBytes(buffer, length);
  return 0;
}

void DTE::togglePower(void) {
  debugPrint(F("Toggle Power"), true);

  digitalWrite(powerPin, HIGH);
  delay(1200);
  digitalWrite(powerPin, LOW);
  powerDown = false;
  flowControl = (struct FlowControl){0, true, 0, true};
  while (ATResponse(3000)) {
    if (isResponseEqual(F("RDY"))) {
      powerDown = false;
      Urc.resetUnsolicitedResultCode();
      if (AT()) return;
    }
    if (isResponseEqual(F("NORMAL POWER DOWN"))) {
      powerDown = true;
      delay(1000);
      return;
    } else {
      unsolicitedResultCode();
    }
  }
  if (AT()) {
    powerDown = false;
    Urc.resetUnsolicitedResultCode();
    if (isEcho())
      setEcho(false);
    if (getFlowControl().dce == 0)
      setFlowControl(1);
    setFlowControlStatusDce(false);
    return;
  } else {
    powerDown = true;
    delay(1000);
    return;
  }
}

void DTE::clearReceivedBuffer(void) {
  debugPrint(F("clearReceivedBuffer"), true);

  if (!isListening()) listen();
  setFlowControlStatusDce(true);
  unsigned long t = millis();
  if (hardwareSerial) {
    while (hardwareSerial->available() == 0 && millis() - t < 50)
      ;
    if (hardwareSerial->available() == 0) {
      setFlowControlStatusDce(false);
      return;
    }
  }
  if (softwareSerial) {
    while (softwareSerial->available() == 0 && millis() - t < 50)
      ;
    if (softwareSerial->available() == 0) {
      setFlowControlStatusDce(false);
      return;
    }
  }
  while (ATResponse(50)) {
    if (isResponseEqual("RDY"))
      echo = true;
    else
      unsolicitedResultCode();
  }
  setFlowControlStatusDce(false);
}

bool DTE::AT(void) {
  const __FlashStringHelper *command = F("AT\r");

  powerDown = false;
  echo = false;
  ATCommand(command);
  while (true) {
    if (!ATResponse()) {
      powerDown = true;
      break;
    }
    if (isResponseEqual(command))
      echo = true;
    else if (isResponseOk())
      return true;
    else
      unsolicitedResultCode();
  }
  powerDown = true;
  return false;
}

bool DTE::ATCommand(const char at[]) {
  if (powerDown) {
    debugPrint(F("Power Down"), true);
    return false;
  }

  debugPrint("Command: ");
  debugPrint(at, true);

  if (!isListening()) listen();
  setFlowControlStatusDce(false);
  if (hardwareSerial) hardwareSerial->print(at);
  if (softwareSerial) softwareSerial->print(at);
  if (echo) {
    if (strlen(at) > (sizeof(response) - 2)) {
      char atEcho[strlen(at) + 3];
      while (true) {
        if (!ATResponse(atEcho, sizeof(atEcho))) return false;
        if (strstr(atEcho, "ERROR") != NULL) return false;
        if (strcmp(atEcho, at) == 0)
          break;
        else
          Urc.unsolicitedResultCode(atEcho);
      }
    } else if (!ATResponseEqual(at))
      return false;
  }
  return true;
}

bool DTE::ATCommand(const __FlashStringHelper *at) {
  char buffer[strlen_P((const char *)at) + 1];
  strcpy_P(buffer, (const char *)at);
  return ATCommand(buffer);
}

bool DTE::ATResponse(char buffer[], size_t bufferSize, unsigned long timeout) {
  if (powerDown) {
    debugPrint(F("Power Down"), true);
    return false;
  }

  setFlowControlStatusDce(true);

  unsigned long t = millis();
  while (true) {
    if (hardwareSerial) {
      if (hardwareSerial->available() > 0) break;
    }
    if (softwareSerial) {
      if (softwareSerial->available() > 0) break;
    }
    if (millis() - t > timeout) {
      debugPrint(F("No response"), true);
      return false;
    }
    delay(1);
  }

  t = millis();
  unsigned int i = 0;
  while (true) {
    if (hardwareSerial) {
      while (hardwareSerial->available() > 0) {
        buffer[i++] = hardwareSerial->read();
        t = millis();
        if (i >= 2) {
          if (buffer[i - 2] == '\r' && buffer[i - 1] == '\n') break;
        }
      }
    }
    if (softwareSerial) {
      while (softwareSerial->available() > 0) {
        buffer[i++] = softwareSerial->read();
        t = millis();
        if (i >= 2) {
          if (buffer[i - 2] == '\r' && buffer[i - 1] == '\n') break;
        }
      }
    }
    if (i >= 2) {
      if (buffer[i - 2] == '\r' && buffer[i - 1] == '\n') {
        buffer[i - 2] = '\0';
        if (strlen(buffer) > 0)
          break;
        else
          i = 0;
      }
    }
    if (millis() - t > 50 || i >= (bufferSize - 1)) {
      buffer[i] = '\0';
      break;
    }
  }
  debugPrint(F("Response: "));
  debugPrint(buffer, true);
  return true;
}

bool DTE::ATResponse(unsigned long timeout) {
  return ATResponse(response, sizeof(response), timeout);
}

bool DTE::ATResponseEqual(const char expected[], unsigned long timeout) {
  while (true) {
    if (!ATResponse(timeout)) return false;
    if (isResponseContain(F("ERROR"))) return false;
    if (isResponseEqual(expected)) break;
    if (isResponseEqual(F("RDY")))
      echo = true;
    else
      unsolicitedResultCode();
  }
  return true;
}

bool DTE::ATResponseEqual(const __FlashStringHelper *expected, unsigned long timeout) {
  char buffer[strlen_P((const char *)expected) + 1];
  strcpy_P(buffer, (const char *)expected);
  return ATResponseEqual(buffer, timeout);
}

bool DTE::ATResponseContain(const char expected[], unsigned long timeout) {
  while (true) {
    if (!ATResponse(timeout)) return false;
    if (isResponseContain(F("ERROR"))) return false;
    if (isResponseContain(expected)) break;
    if (isResponseEqual(F("RDY")))
      echo = true;
    else
      unsolicitedResultCode();
  }
  return true;
}

bool DTE::ATResponseContain(const __FlashStringHelper *expected, unsigned long timeout) {
  char buffer[strlen_P((const char *)expected) + 1];
  strcpy_P(buffer, (const char *)expected);
  return ATResponseContain(buffer, timeout);
}

bool DTE::ATResponseOk(unsigned long timeout) {
  ATResponse(timeout);
  return isResponseOk();
}

bool DTE::isResponseEqual(const char expected[]) {
  if (strcmp(response, expected) != 0) return false;
  return true;
}

bool DTE::isResponseEqual(const __FlashStringHelper *expected) {
  char buffer[strlen_P((const char *)expected) + 1];
  strcpy_P(buffer, (const char *)expected);
  return isResponseEqual(buffer);
}

bool DTE::isResponseContain(const char expected[]) {
  if (strstr(response, expected) == NULL) return false;
  return true;
}

bool DTE::isResponseContain(const __FlashStringHelper *expected) {
  char buffer[strlen_P((const char *)expected) + 1];
  strcpy_P(buffer, (const char *)expected);
  return isResponseContain(buffer);
}

bool DTE::isResponseOk(void) {
  if (!(isResponseEqual(F("OK")) || isResponseEqual(F("OK\r")))) return false;
  if (available()) clearReceivedBuffer();
  setFlowControlStatusDce(false);
  return true;
}

bool DTE::unsolicitedResultCode(void) {
  debugPrint(F("URC"), true);
  return Urc.unsolicitedResultCode(response);
}

bool DTE::setEcho(bool echo) {
  if (this->echo == echo) return true;
  if (!atSetCommandEchoMode(echo)) return false;
  return true;
}

const char *DTE::getProductSerialNumberIdentification(void) {
  if (strlen(productSerialNumberIdentification) == 0) atRequestProductSerialNumberIdentification();
  return productSerialNumberIdentification;
}

struct FlowControl DTE::getFlowControl(void) {
  if (!atSetLocalDataFlowControl()) return (struct FlowControl){0, true, 0, true};
  return this->flowControl;
}

bool DTE::setFlowControl(unsigned char dce, unsigned char dte) {
  if (!atSetLocalDataFlowControl(dce, dte)) return false;
  if (flowControl.dce == 1) setFlowControlStatusDce(false);
  return true;
}

bool DTE::setFlowControlStatusDce(bool on) {
  if (flowControl.dce != 1) return false;
  if (flowControl.dceOn == on) return true;
  if (hardwareSerial) hardwareSerial->write(on ? 17 : 19);
  if (softwareSerial) softwareSerial->write(on ? 17 : 19);
  flowControl.dceOn = on;
  return true;
}

long DTE::getBaudrate(void) {
  if (!atSetLocalDataFlowControl()) return 0;
  return baudrate;
}

bool DTE::powerReset(void) {
  togglePower();
  while (powerDown) {
    togglePower();
  }
  return true;
}
