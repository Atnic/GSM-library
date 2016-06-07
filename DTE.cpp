#include "DTE.h"
#include "URC.h"

void DTE::debugPrint(const char message[], bool returnChar) {
	if(debug) {
		Serial.print(message);
		if(returnChar) Serial.println();
	}
}

void DTE::debugPrint(const __FlashStringHelper *message, bool returnChar) {
	char buffer[strlen_P((const char *)message)+1];
	strcpy_P(buffer, (const char *)message);
	debugPrint(buffer, returnChar);
}

/* DTE Class */
DTE::DTE(HardwareSerial &hardwareSerial, int powerPin, bool debug) {
	this->hardwareSerial = &hardwareSerial;
	this->powerPin = powerPin;
	this->debug = debug;

	pinMode(this->powerPin, OUTPUT);
	digitalWrite(this->powerPin, LOW);
}

DTE::DTE(SoftwareSerial &softwareSerial, int powerPin, bool debug) {
	this->softwareSerial = &softwareSerial;
	this->powerPin = powerPin;
	this->debug = debug;

	pinMode(this->powerPin, OUTPUT);
	digitalWrite(this->powerPin, LOW);
}

int DTE::available(void) {
	if(hardwareSerial) return hardwareSerial->available();
	if(softwareSerial) return softwareSerial->available();
	return -1;
}

bool DTE::isListening(void) {
	if(softwareSerial) return softwareSerial->isListening();
	return false;
}

bool DTE::listen(void) {
	if(softwareSerial) return softwareSerial->listen();
	return false;
}

size_t DTE::write(const char str[]) {
	if(hardwareSerial) return hardwareSerial->write(str);
	if(softwareSerial) return softwareSerial->write(str);
	return 0;
}

size_t DTE::readBytes(char buffer[], size_t length) {
	if(hardwareSerial) return hardwareSerial->readBytes(buffer, length);
	if(softwareSerial) return softwareSerial->readBytes(buffer, length);
	return 0;
}

void DTE::togglePower(void) {
	debugPrint(F("Toggle Power"), true);

	digitalWrite(powerPin, HIGH);
	delay(1200);
	digitalWrite(powerPin, LOW);
	powerDown = false;
	while(ATResponse(5000)) {
		if(isResponseEqual(F("RDY"))) {
			echo = true;
			powerDown = false;
			Urc.resetUnsolicitedResultCode();
			return;
		}
		if(isResponseEqual(F("NORMAL POWER DOWN"))) {
			powerDown = true;
			delay(1000);
			return;
		}
		else {
			unsolicitedResultCode();
		}
	}
	if (AT()) {
		echo = true;
		powerDown = false;
		Urc.resetUnsolicitedResultCode();
		return;
	}
	else {
		powerDown = true;
		delay(1000);
		return;
	}
}

void DTE::clearReceivedBuffer(void) {
	debugPrint(F("clearReceivedBuffer"), true);
	if(hardwareSerial) {
		if(hardwareSerial->available() == 0) return;
	}
	if(softwareSerial) {
		if(softwareSerial->available() == 0) return;
	}
	while (ATResponse(50)) {
		if(isResponseEqual("RDY")) echo = true;
		else unsolicitedResultCode();
	}
}

bool DTE::AT(void) {
	powerDown = false;
	echo = false;
	ATCommand(F("AT\r"));
	while(true) {
		if(!ATResponse()) {
			powerDown = true;
			break;
		}
		if(isResponseEqual(F("AT\r"))) echo = true;
		else if(isResponseEqual(F("OK"))) return true;
		else unsolicitedResultCode();
	}
	return false;
}

bool DTE::ATCommand(const char at[]) {
	debugPrint("Command: ");
	debugPrint(at, true);
	if(hardwareSerial) {
		hardwareSerial->print(at);
	}
	if(softwareSerial) {
		softwareSerial->print(at);
	}
	if(echo)
		if(!ATResponseEqual(at)) return false;
	return true;
}

bool DTE::ATCommand(const __FlashStringHelper *at) {
	char buffer[strlen_P((const char *)at)+1];
	strcpy_P(buffer, (const char *)at);
	return ATCommand(buffer);
}

bool DTE::ATResponse(unsigned long timeout) {
	if(powerDown) {
		debugPrint(F("Power Down"), true);
		return false;
	}

	unsigned long t = millis();
	while(true) {
		if(hardwareSerial) {
			if (hardwareSerial->available() > 0) {
				break;
			}
		}
		if(softwareSerial) {
			if (softwareSerial->available() > 0) {
				break;
			}
		}
		if(millis() - t > timeout) {
			debugPrint(F("No response"), true);
			return false;
		}
	}

	t = millis();
	unsigned int i = 0;
	while(true) {
		if(hardwareSerial) {
			if (hardwareSerial->available() > 0) {
				response[i++] = hardwareSerial->read();
				t = millis();
			}
		}
		if(softwareSerial) {
			if (softwareSerial->available() > 0) {
				response[i++] = softwareSerial->read();
				t = millis();
			}
		}
		if(i >= 2) {
			if(response[i-2] == '\r' && response[i-1] == '\n') {
				response[i-2] = '\0';
				if(strlen(response) > 0) {
					break;
				}
				else i = 0;
			}
		}
		if(millis() - t > 50 || i >= (sizeof(response)-1)) {
			response[i] = '\0';
			break;
		}
	}
	debugPrint(F("Response: "));
	debugPrint(response, true);
	return true;
}

bool DTE::ATResponseEqual(const char expected[], unsigned long timeout) {
	while(true) {
		if(!ATResponse(timeout)) return false;
		if(isResponseEqual(F("ERROR"))) return false;
		if(isResponseEqual(expected)) break;
		if(isResponseEqual(F("RDY"))) echo = true;
		else unsolicitedResultCode();
	}
	return true;
}

bool DTE::ATResponseEqual(const __FlashStringHelper *expected, unsigned long timeout) {
	char buffer[strlen_P((const char *)expected)+1];
	strcpy_P(buffer, (const char *)expected);
	return ATResponseEqual(buffer, timeout);
}

bool DTE::ATResponseContain(const char expected[], unsigned long timeout) {
	while(true) {
		if(!ATResponse(timeout)) return false;
		if(isResponseContain(F("ERROR"))) return false;
		if(isResponseContain(expected)) break;
		if(isResponseEqual(F("RDY"))) echo = true;
		else unsolicitedResultCode();
	}
	return true;
}

bool DTE::ATResponseContain(const __FlashStringHelper *expected, unsigned long timeout) {
	char buffer[strlen_P((const char *)expected)+1];
	strcpy_P(buffer, (const char *)expected);
	return ATResponseContain(buffer, timeout);
}

bool DTE::ATResponseOk(unsigned long timeout) {
	return ATResponseEqual(F("OK"), timeout);
}

bool DTE::isResponseEqual(const char expected[]) {
	if(strcmp(response, expected) != 0)
	return false;
	return true;
}

bool DTE::isResponseEqual(const __FlashStringHelper *expected) {
	char buffer[strlen_P((const char *)expected)+1];
	strcpy_P(buffer, (const char *)expected);
	return isResponseEqual(buffer);
}

bool DTE::isResponseContain(const char expected[]) {
	if(strstr(response, expected) == NULL)
		return false;
	return true;
}

bool DTE::isResponseContain(const __FlashStringHelper *expected) {
	char buffer[strlen_P((const char *)expected)+1];
	strcpy_P(buffer, (const char *)expected);
	return isResponseContain(buffer);
}

bool DTE::isResponseOk(void) {
	return isResponseContain(F("OK"));
}

bool DTE::unsolicitedResultCode(void) {
	debugPrint(F("URC: "));
	debugPrint(response, true);
  return Urc.unsolicitedResultCode(response);
}

bool DTE::powerReset(void) {
	togglePower();
	while (powerDown) {
		togglePower();
	}
	powerDown = false;
	return true;
}
