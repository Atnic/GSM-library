#include "GSM.h"
#include "URC.h"

bool GSM::atReIssueLastCommand(void) {
	const __FlashStringHelper *command = F("A/\r");

	dte->clearReceivedBuffer();
  if(!dte->ATCommand(command)) return false;
	if(!dte->ATResponseOk()) return false;
	return true;
}

bool GSM::atSetCommandEchoMode(bool echo) {
	const __FlashStringHelper *command = F("ATE%d\r");
	char buffer[8]; // "ATEX\r\r\n"

	sprintf_P(buffer, (const char *)command, (echo)?1:0);

	dte->clearReceivedBuffer();
  if(!dte->ATCommand(buffer)) return false;
	if(!dte->ATResponseOk()) return false;
	dte->setEcho(echo);
	return true;
}

bool GSM::atSetFixedLocalRate(void) {
	const __FlashStringHelper *command = F("AT+IPR?\r");
	const __FlashStringHelper *response = F("+IPR: ");
	long baudrate;

	dte->clearReceivedBuffer();
  if(!dte->ATCommand(command)) return false;
	if(!dte->ATResponseContain(response)) return false;
	char *str = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
	baudrate = atol(str);
	if(!dte->ATResponseOk()) return false;
	this->baudrate = baudrate;
	return true;
}

bool GSM::atSetFixedLocalRate(long baudrate) {
	const __FlashStringHelper *command = F("AT+IPR=%ld\r");
	char buffer[17]; // "AT+IPR=XXXXXX\r\r\n"

	sprintf_P(buffer, (const char *)command, baudrate);

	dte->clearReceivedBuffer();
  if(!dte->ATCommand(buffer)) return false;
	if(!dte->ATResponseOk()) return false;
	this->baudrate = baudrate;
	return true;
}

bool GSM::atOperatorSelection(void) {
	const __FlashStringHelper *command = F("AT+COPS?\r");
	const __FlashStringHelper *response = F("+COPS: ");
	struct Operator selectedOperator;

	dte->clearReceivedBuffer();
  if(!dte->ATCommand(command)) return false;
	if(!dte->ATResponseContain(response)) return false;
	char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
	char *str = strtok(pointer, ",\"");
	selectedOperator = this->selectedOperator;
	for (unsigned char i = 0; i < 3 && str != NULL; i++) {
		if(i == 0) selectedOperator.mode = str[0] - '0';
		if(i == 1) selectedOperator.format = str[0] - '0';
		if(i == 2) {
			if(selectedOperator.format == 2)
				strcpy(selectedOperator.operNumeric, str);
			else
				strcpy(selectedOperator.oper, str);
		}
		str = strtok(NULL, ",\"");
	}
	if(!dte->ATResponseOk()) return false;
	this->selectedOperator = selectedOperator;
	return true;
}

bool GSM::atOperatorSelection(unsigned char mode, unsigned char format, const char oper[]) {
	char buffer[13 + strlen(oper)]; // "AT+COPS=X,X,{oper}\r\r\n"

	if(mode == 0) {
		const __FlashStringHelper *command = F("AT+COPS=%d\r");
		sprintf_P(buffer, (const char *)command, mode);
	}
	else if((mode == 1 || mode == 4) && format < 3 && strlen(oper) > 0) {
		const __FlashStringHelper *command = F("AT+COPS=%d,%d,\"%s\"\r");
		sprintf_P(buffer, (const char *)command, mode, format, oper);
  }
	else if(mode == 3 && format < 3) {
		const __FlashStringHelper *command = F("AT+COPS=%d,%d\r");
		sprintf_P(buffer, (const char *)command, mode, format);
	}
	else
		return false;

	dte->clearReceivedBuffer();
  if(!dte->ATCommand(buffer)) return false;
	if(!dte->ATResponseOk()) return false;
	return true;
}

bool GSM::atOperatorSelection(unsigned char mode, unsigned char format, const __FlashStringHelper *oper) {
	char buffer[strlen_P((const char *)oper)+1];
	strcpy_P(buffer, (const char *)oper);
	return atOperatorSelection(mode, format, buffer);
}

bool GSM::atSelectPhonebookMemoryStorage(void) {
	const __FlashStringHelper *command = F("AT+CPBS?\r");
	const __FlashStringHelper *response = F("+CPBS: ");
	struct PhonebookMemoryStorage phonebookMemoryStorage;

	dte->clearReceivedBuffer();
  if(!dte->ATCommand(command)) return false;
	if(!dte->ATResponseContain(response)) return false;
	char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
	char *str = strtok(pointer, ",\"");
	for (unsigned char i = 0; i < 3 && str != NULL; i++) {
		if(i == 0) strcpy(phonebookMemoryStorage.storage, str);
		if(i == 1) phonebookMemoryStorage.used = atoi(str);
		if(i == 2) phonebookMemoryStorage.total = atoi(str);
		str = strtok(NULL, ",\"");
	}
	if(!dte->ATResponseOk()) return false;
	this->phonebookMemoryStorage = phonebookMemoryStorage;
	return true;
}

bool GSM::atSelectPhonebookMemoryStorage(const char storage[]) {
	const __FlashStringHelper *command = F("AT+CPBS=\"%s\"\r");
	char buffer[14 + strlen(storage)]; // "AT+CPBS=\"{storage}\"\r\r\n"

	sprintf_P(buffer, (const char *)command, storage);

	dte->clearReceivedBuffer();
  if(!dte->ATCommand(buffer)) return false;
	if(!dte->ATResponseOk()) return false;
	return true;
}

bool GSM::atEnterPIN(void) {
	const __FlashStringHelper *command = F("AT+CPIN?\r");
	const __FlashStringHelper *response = F("+CPIN: ");

	dte->clearReceivedBuffer();
  if(!dte->ATCommand(command)) return false;
	if(!dte->ATResponseContain(response, 1000)) return false;
	char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
	char *str = strtok(pointer, "\"");
	if(!dte->ATResponseOk()) return false;
	strcpy(pinStatus, str);
	return true;
}

bool GSM::atNetworkRegistration(void) {
	const __FlashStringHelper *command = F("AT+CGREG?\r");
	const __FlashStringHelper *response = F("+CGREG: ");
	struct NetworkRegistration networkRegistration;

	dte->clearReceivedBuffer();
  if(!dte->ATCommand(command)) return false;
	if(!dte->ATResponseContain(response)) return false;
	char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
	char *str = strtok(pointer, ",\"");
	for (unsigned char i = 0; i < 4 && str != NULL; i++) {
		if(i == 0) networkRegistration.n = atoi(str);
		if(i == 1) networkRegistration.status = atoi(str);
		if(i == 2) strcpy(networkRegistration.lac, str);
		if(i == 3) strcpy(networkRegistration.ci, str);
		str = strtok(NULL, ",\"");
	}
	if(!dte->ATResponseOk()) return false;
	this->networkRegistration = networkRegistration;
	return true;
}

bool GSM::atNetworkRegistration(unsigned char n) {
	const __FlashStringHelper *command = F("AT+CGREG=%d\r");
	char buffer[11]; // "AT+CGREG=X\r\r\n"

	sprintf_P(buffer, (const char *)command, n);

	dte->clearReceivedBuffer();
  if(!dte->ATCommand(buffer)) return false;
	if(!dte->ATResponseOk()) return false;
	return true;
}

bool GSM::atSignalQualityReport(void) {
	const __FlashStringHelper *command = F("AT+CSQ\r");
	const __FlashStringHelper *response = F("+CSQ: ");
	struct SignalQuality signalQuality;

	dte->clearReceivedBuffer();
  if(!dte->ATCommand(command)) return false;
	if(!dte->ATResponseContain(response)) return false;
	char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
	char *str = strtok(pointer, ",");
	for (unsigned char i = 0; i < 2 && str != NULL; i++) {
		if(i == 0) signalQuality.rssi = atoi(str);
		if(i == 1) signalQuality.ber = atoi(str);
		str = strtok(NULL, ",");
	}
	if(!dte->ATResponseOk()) return false;
	this->signalQuality = signalQuality;
	return true;
}

bool GSM::atSubscriberNumber(void) {
	const __FlashStringHelper *command = F("AT+CNUM\r");
	const __FlashStringHelper *response = F("+CNUM: ");
	struct SubscriberNumber subscriberNumber;

	dte->clearReceivedBuffer();
  if(!dte->ATCommand(command)) return false;
	if(!dte->ATResponseContain(response)) return false;
	char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
	char *str = strtok(pointer, ",");
	for (unsigned char i = 0; i < 5 && str != NULL; i++) {
		if(i == 0) strcpy(subscriberNumber.characterSet, str);
		if(i == 1) strcpy(subscriberNumber.number, str);
		if(i == 2) subscriberNumber.type = atoi(str);
		if(i == 3) subscriberNumber.speed = str[0] - '0';
		if(i == 4) subscriberNumber.service = str[0] - '0';
		str = strtok(NULL, ",");
	}
	strcpy(subscriberNumber.characterSet, strtok(subscriberNumber.characterSet, "\""));
	strcpy(subscriberNumber.number, strtok(subscriberNumber.number, "\""));
	if(!dte->ATResponseOk()) return false;
	this->subscriberNumber = subscriberNumber;
	return true;
}

bool GSM::atClock(void) {
	const __FlashStringHelper *command = F("AT+CCLK?\r");
	const __FlashStringHelper *response = F("+CCLK: ");

	dte->clearReceivedBuffer();
  if(!dte->ATCommand(command)) return false;
	if(!dte->ATResponseContain(response)) return false;
	char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
	char *str = strtok(pointer, "\"");
	if(!dte->ATResponseOk()) return false;
	strcpy(clock.timestamp, str);
	str = strtok(str, "/,:+");
	for (unsigned char i = 0; i < 7 && str != NULL; i++) {
		if(i == 0) clock.year = atoi(str);
		if(i == 1) clock.month = atoi(str);
		if(i == 2) clock.day = atoi(str);
		if(i == 3) clock.hour = atoi(str);
		if(i == 4) clock.minute = atoi(str);
		if(i == 5) clock.second = atoi(str);
		if(i == 6) clock.timezone = atoi(str)/4;
		str = strtok(NULL, "/,:+");
	}
	return true;
}

bool GSM::atClock(const char timestamp[]) {
	const __FlashStringHelper *command = F("AT+CCLK=\"%s\"\r");
	char buffer[14 + strlen(timestamp)]; // "AT+CCLK=\"{timestamp}\"\r\r\n"

	sprintf_P(buffer, (const char *)command, timestamp);

	dte->clearReceivedBuffer();
  if(!dte->ATCommand(buffer)) return false;
	if(!dte->ATResponseOk()) return false;
	return true;
}

bool GSM::atBatteryCharge(void) {
	const __FlashStringHelper *command = F("AT+CBC\r");
	const __FlashStringHelper *response = F("+CBC: ");
	struct BatteryStatus batteryStatus;

	dte->clearReceivedBuffer();
  if(!dte->ATCommand(command)) return false;
	if(!dte->ATResponseContain(response)) return false;
	char *pointer = strstr_P(dte->getResponse(), (const char *)response) + strlen_P((const char *)response);
	char *str = strtok(pointer, ",");
	unsigned char i = 0;
	for (i = 0; i < 3 && str != NULL; i++) {
		if(i == 0) batteryStatus.charge = (str[0] == '1')?true:false;
		if(i == 1) batteryStatus.capacityLevel = atoi(str);
		if(i == 2) batteryStatus.voltage = (float)atoi(str) * 0.001;
		str = strtok(NULL, ",");
	}
	if(!dte->ATResponseOk()) return false;
	this->batteryStatus = batteryStatus;
	return true;
}

/* GSM Class */
GSM::GSM(DTE &dte) {
	this->dte = &dte;
}

long GSM::getBaudrate(void) {
	if(baudrate < 0)
		atSetFixedLocalRate();
	return baudrate;
}

struct Operator GSM::getOperator(unsigned char format) {
	if(strlen(selectedOperator.operNumeric) == 0) {
		if(getNetworkRegistration().status == 1) {
			atOperatorSelection(3, 2);
			atOperatorSelection();
		}
	}
	if(format >= 2)
		format = 0;
	if(format < 2) {
		if(selectedOperator.format != format || strlen(selectedOperator.oper) == 0) {
			if(getNetworkRegistration().status == 1) {
				atOperatorSelection(3, format);
				atOperatorSelection();
			}
		}
	}
	return selectedOperator;
}

struct PhonebookMemoryStorage GSM::getPhonebookMemoryStrorage(void) {
	if(getNetworkRegistration().status == 1)
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
		if(strcmp(getPinStatus(), "READY") == 0) {
			t = millis();
			atClock();
		}
	}
	return clock;
}

struct SubscriberNumber GSM::getSubscriberNumber(void) {
	if(strlen(subscriberNumber.number) == 0)
		if(getNetworkRegistration().status == 1)
			atSubscriberNumber();
	return subscriberNumber;
}

struct BatteryStatus GSM::getBatteryStatus(void) {
	atBatteryCharge();
	return batteryStatus;
}
