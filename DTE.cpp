#include "DTE.h"
#include "URC.h"

/* DTE Class */
DTE::DTE(HardwareSerial &hardwareSerial, int powerPin, bool debug)
{
	this->hardwareSerial = &hardwareSerial;
	this->powerPin = powerPin;
	this->debug = debug;
}

DTE::DTE(SoftwareSerial &softwareSerial, int powerPin, bool debug)
{
	this->softwareSerial = &softwareSerial;
	this->powerPin = powerPin;
	this->debug = debug;
}

int DTE::available(void)
{
	if(hardwareSerial)
		return hardwareSerial->available();
	if(softwareSerial)
		return softwareSerial->available();
	return -1;
}

bool DTE::isListening(void)
{
	if(softwareSerial)
		return softwareSerial->isListening();
	return false;
}

bool DTE::listen(void)
{
	if(softwareSerial)
		return softwareSerial->listen();
	return false;
}

void DTE::clearReceivedBuffer(void)
{
	debugPrint(F("clearReceivedBuffer"), true);
	// unsigned long t;
	if(hardwareSerial) {
		if(hardwareSerial->available() == 0) return;
		// t = millis();
		// while(millis() - t < 200) {
		// 	if (hardwareSerial->available() > 0) {
		// 		hardwareSerial->read();
		// 		t = millis();
		// 	}
		// }
	}
	if(softwareSerial) {
		if(softwareSerial->available() == 0) return;
		// t = millis();
		// while(millis() - t < 200) {
		// 	if (softwareSerial->available() > 0) {
		// 		softwareSerial->read();
		// 		t = millis();
		// 	}
		// }
	}
	while (ATResponse(50l)) {
		if(isResponse("RDY\r\n")) setEcho(true);
		else urc.unsolicitedResultCode(getResponse());
	}
}

void DTE::debugPrint(const char message[], bool returnChar)
{
	if(debug) {
		Serial.print(message);
		if(returnChar) Serial.println();
		// Serial.flush();
	}
}

void DTE::debugPrint(const __FlashStringHelper message[], bool returnChar)
{
	char buffer[strlen_P((const char *)message)+1];
	strcpy_P(buffer, (const char *)message);
	debugPrint(buffer, returnChar);
}

void DTE::togglePower(void) {
	debugPrint(F("Toggle Power"), true);
	digitalWrite(powerPin, HIGH);
	delay(1200);
	digitalWrite(powerPin, LOW);
	setPowerDown(false);
	while(ATResponse(5000)) {
		if(isResponse(F("RDY\r\n"))) {
			setEcho(true);
			setPowerDown(false);
			break;
		}
		if(isResponse(F("NORMAL POWER DOWN\r\n"))) {
			setPowerDown(true);
			delay(1000);
			break;
		}
	}
}

void DTE::begin(long speed)
{
	if(hardwareSerial)
	{
		hardwareSerial->begin(speed);
		hardwareSerial->setTimeout(1000);
	}
	if(softwareSerial)
	{
		softwareSerial->begin(speed);
		softwareSerial->setTimeout(1000);
	}
}

bool DTE::AT(void)
{
	ATCommand(F("AT\r"));
	while(ATResponse()) {
		if(isResponse(F("AT\r\r\n"))) setEcho(true);
		else if(isResponse(F("OK\r\n"))) {
			setPowerDown(false);
			return true;
		}
		else urc.unsolicitedResultCode(getResponse());
	}
	return false;
}

void DTE::ATCommand(const char at[])
{
	debugPrint(at, true);
	if(hardwareSerial) {
		hardwareSerial->print(at);
  }
  if(softwareSerial) {
		softwareSerial->print(at);
  }
}

void DTE::ATCommand(const __FlashStringHelper at[])
{
	char buffer[strlen_P((const char *)at)+1];
	strcpy_P(buffer, (const char *)at);
	ATCommand(buffer);
}

bool DTE::ATResponse(unsigned long timeout)
{
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
		if(i > 1) {
			if(response[i-2] == '\r' && response[i-1] == '\n') {
				response[i] = '\0';
				if(strlen(response) > 2) {
					break;
				}
				else i = 0;
			}
		}
		if(millis() - t > 50 || i >= (sizeof(response)-3)) {
			response[i++] = '\r';
			response[i++] = '\n';
			response[i] = '\0';
			break;
		}
	}
	debugPrint(F("Response: "));
	debugPrint(response);
	return true;
}

bool DTE::ATResponseEqual(const char expected[], unsigned long timeout)
{
	while(true) {
		if(!ATResponse(timeout)) return false;
		if(isResponse(F("ERROR\r\n"))) return false;
		if(isResponse(expected)) break;
		if(isResponse("RDY\r\n")) setEcho(true);
		else urc.unsolicitedResultCode(getResponse());
	}
	return true;
}

bool DTE::ATResponseEqual(const __FlashStringHelper expected[], unsigned long timeout)
{
	char buffer[strlen_P((const char *)expected)+1];
	strcpy_P(buffer, (const char *)expected);
	return ATResponseEqual(buffer, timeout);
}

bool DTE::ATResponseContain(const char expected[], unsigned long timeout)
{
	while(true) {
		if(!ATResponse(timeout)) return false;
		if(isResponseContain(F("ERROR\r\n"))) return false;
		if(isResponseContain(expected)) break;
		if(isResponse("RDY\r\n")) setEcho(true);
		else urc.unsolicitedResultCode(getResponse());
	}
	return true;
}

bool DTE::ATResponseContain(const __FlashStringHelper expected[], unsigned long timeout)
{
	char buffer[strlen_P((const char *)expected)+1];
	strcpy_P(buffer, (const char *)expected);
	return ATResponseContain(buffer, timeout);
}

bool DTE::powerReset(void) {
	togglePower();
	while (isPowerDown()) {
		togglePower();
	}
	setPowerDown(false);
	return true;
}

bool DTE::isResponse(const char expected[])
{
	if(strcmp(getResponse(), expected) != 0)
		return false;
	return true;
}

bool DTE::isResponse(const __FlashStringHelper expected[])
{
	char buffer[strlen_P((const char *)expected)+1];
	strcpy_P(buffer, (const char *)expected);
	return isResponse(buffer);
}

bool DTE::isResponseContain(const char expected[])
{
	if(strstr(getResponse(), expected) == NULL)
		return false;
	return true;
}

bool DTE::isResponseContain(const __FlashStringHelper expected[])
{
	char buffer[strlen_P((const char *)expected)+1];
	strcpy_P(buffer, (const char *)expected);
	return isResponseContain(buffer);
}
