#include "GSM.h"
#include "URC.h"

/* GSM Class */
GSM::GSM(DTE &dte)
{
	this->dte = &dte;
}

bool GSM::atReIssueLastCommand(void) {
	dte->clearReceivedBuffer();
  dte->ATCommand(F("A/\r"));
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(F("A/\r\r\n"))) return false;
	}
	if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
	return true;
}

bool GSM::atSetCommandEchoMode(bool echo) {
	char buffer[strlen_P((const char *)F("ATEX\r\r\n")) + 1];

	sprintf(buffer, "ATE%d\r", (echo)?1:0);

	dte->clearReceivedBuffer();
	dte->ATCommand(buffer);
	if(!dte->ATResponse()) return false;
	while(!dte->isResponseContain(F("ATE")) && !dte->isResponse(F("OK\r\n"))) {
		urc.unsolicitedResultCode(dte->getResponse());
		if(!dte->ATResponse()) return false;
	}
	if(!dte->isResponse(F("OK\r\n")))
		if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
	dte->setEcho(echo);
	return true;
}

bool GSM::atSetFixedLocalRate(void) {
	long baudrate;

	dte->clearReceivedBuffer();
  dte->ATCommand(F("AT+IPR?\r"));
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(F("AT+IPR?\r\r\n"))) return false;
	}
	if(!dte->ATResponseContain(F("+IPR: "))) return false;
	char *pointer = strstr(dte->getResponse(), "+IPR: ") + strlen_P((const char *)F("+IPR: "));
	char *str = strtok(pointer, "\r\n");
	baudrate = atol(str);
	if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
	this->baudrate = baudrate;
	return true;
}

bool GSM::atSetFixedLocalRate(long baudrate) {
	char buffer[strlen_P((const char *)F("AT+IPR=XXXXXX\r\r\n")) + 1];

	sprintf(buffer, "AT+IPR=%ld\r", baudrate);

	dte->clearReceivedBuffer();
	dte->ATCommand(buffer);
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(strcat(buffer, "\r\n"))) return false;
	}
	if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
	this->baudrate = baudrate;
	return true;
}

bool GSM::atOperatorSelection(void) {
	struct Operator selectedOperator;

	dte->clearReceivedBuffer();
  dte->ATCommand(F("AT+COPS?\r"));
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(F("AT+COPS?\r\r\n"))) return false;
	}
	if(!dte->ATResponseContain(F("+COPS: "))) return false;
	char *pointer = strstr(dte->getResponse(), "+COPS: ") + strlen_P((const char *)F("+COPS: "));
	char *str = strtok(pointer, ",\"\r\n");
	int i = 0;
	for (i = 0; i < 3 && str != NULL; i++) {
		// if(i == 0) mode = str[0];
		if(i == 1) selectedOperator.format = str[0] - '0';
		if(i == 2) {
			if(selectedOperator.format == 2)
				strcpy(selectedOperator.operNumeric, str);
			else
				strcpy(selectedOperator.oper, str);
		}
		str = strtok(NULL, ",\"\r\n");
	}
	if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
	if(i > 1) {
		this->selectedOperator.format = selectedOperator.format;
		if(selectedOperator.format == 2)
			strcpy(this->selectedOperator.operNumeric, selectedOperator.operNumeric);
		else
			strcpy(this->selectedOperator.oper, selectedOperator.oper);
	}
	else {
		this->selectedOperator.format = 0;
		strcpy(this->selectedOperator.oper, "");
		strcpy(this->selectedOperator.operNumeric, "");
	}
	return true;
}

bool GSM::atOperatorSelection(unsigned char mode, unsigned char format, const char oper[]) {
	char buffer[strlen_P((const char *)F("AT+COPS=X,X,")) + strlen(oper) + strlen_P((const char *)F("\r\r\n")) + 1];
	char command[] = "AT+COPS=";

	if(mode == 0) {
		sprintf(buffer, "%s%d\r", command, mode);
	}
	else if((mode == 1 || mode == 4) && format < 3 && strlen(oper) > 0) {
		if(format == 2)
			sprintf(buffer, "%s%d,%d,%s\r", command, mode, format, oper);
		else
			sprintf(buffer, "%s%d,%d,\"%s\"\r", command, mode, format, oper);
  }
	else if(mode == 3 && format < 3) {
		sprintf(buffer, "%s%d,%d\r", command, mode, format);
	}
	else
		return false;

	dte->clearReceivedBuffer();
	dte->ATCommand(buffer);
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(strcat(buffer, "\r\n"))) return false;
	}
	if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
	return true;
}

bool GSM::atOperatorSelection(unsigned char mode, unsigned char format, const __FlashStringHelper oper[]) {
	char buffer[strlen_P((const char *)oper)+1];
	strcpy_P(buffer, (const char *)oper);
	return atOperatorSelection(mode, format, buffer);
}

bool GSM::atOperatorSelection(unsigned char mode, unsigned char format, unsigned char oper) {
	char buffer[3];
	itoa(oper, buffer, 10);
	return atOperatorSelection(mode, format, buffer);
}

bool GSM::atSelectPhonebookMemoryStorage(void) {
	struct PhonebookMemoryStorage phonebookMemoryStorage;

	dte->clearReceivedBuffer();
	dte->ATCommand(F("AT+CPBS?\r"));
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(F("AT+CPBS?\r\r\n"))) return false;
	}
	if(!dte->ATResponseContain(F("+CPBS: "))) return false;
	char *pointer = strstr(dte->getResponse(), "+CPBS: ") + strlen_P((const char *)F("+CPBS: "));
	char *str = strtok(pointer, ",\"\r\n");
	int i = 0;
	for (i = 0; i < 3 && str != NULL; i++) {
		if(i == 0) strcpy(phonebookMemoryStorage.storage, str);
		if(i == 1) phonebookMemoryStorage.used = atoi(str);
		if(i == 2) phonebookMemoryStorage.total = atoi(str);
		str = strtok(NULL, ",\"\r\n");
	}
	if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
	this->phonebookMemoryStorage = phonebookMemoryStorage;
	return true;
}

bool GSM::atSelectPhonebookMemoryStorage(const char storage[]) {
	char buffer[strlen_P((const char *)F("AT+CPBS=\"\"\r\r\n")) + strlen(storage) + 1];

	sprintf(buffer, "AT+CPBS=\"%s\"\r", storage);

	dte->clearReceivedBuffer();
	dte->ATCommand(buffer);
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(strcat(buffer, "\r\n"))) return false;
	}
	if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
	return true;
}

bool GSM::atNetworkRegistration(void) {
	struct NetworkRegistration networkRegistration;

	dte->clearReceivedBuffer();
	dte->ATCommand(F("AT+CGREG?\r"));
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(F("AT+CGREG?\r\r\n"))) return false;
	}
	if(!dte->ATResponseContain(F("+CGREG: "))) return false;
	char *pointer = strstr(dte->getResponse(), "+CGREG: ") + strlen_P((const char *)F("+CGREG: "));
	char *str = strtok(pointer, ",\"\r\n");
	unsigned char i = 0;
	for (i = 0; i < 4 && str != NULL; i++) {
		if(i == 0) networkRegistration.n = atoi(str);
		if(i == 1) networkRegistration.status = atoi(str);
		if(i == 2) strcpy(networkRegistration.lac, str);
		if(i == 3) strcpy(networkRegistration.ci, str);
		str = strtok(NULL, ",\"\r\n");
	}
	if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
	this->networkRegistration = networkRegistration;
	return true;
}

bool GSM::atNetworkRegistration(unsigned char n) {
	char buffer[strlen_P((const char *)F("AT+CGREG=X\r\r\n")) + 1];

	sprintf(buffer, "AT+CGREG=%d\r", n);

	dte->clearReceivedBuffer();
	dte->ATCommand(buffer);
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(strcat(buffer, "\r\n"))) return false;
	}
	if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
	return true;
}

bool GSM::atSignalQualityReport(void) {
	struct SignalQuality signalQuality;

	dte->clearReceivedBuffer();
	dte->ATCommand(F("AT+CSQ\r"));
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(F("AT+CSQ\r\r\n"))) return false;
	}
	if(!dte->ATResponseContain(F("+CSQ: "))) return false;
	char *pointer = strstr(dte->getResponse(), "+CSQ: ") + strlen_P((const char *)F("+CSQ: "));
	char *str = strtok(pointer, ",\r\n");
	unsigned char i = 0;
	for (i = 0; i < 2 && str != NULL; i++) {
		if(i == 0) signalQuality.rssi = atoi(str);
		if(i == 1) signalQuality.ber = atoi(str);
		str = strtok(NULL, ",\r\n");
	}
	if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
	this->signalQuality = signalQuality;
	return true;
}

bool GSM::atSubscriberNumber(void) {
	struct SubscriberNumber subscriberNumber;

	dte->clearReceivedBuffer();
	dte->ATCommand(F("AT+CNUM\r"));
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(F("AT+CNUM\r\r\n"))) return false;
	}
	if(!dte->ATResponseContain(F("+CNUM: "))) return false;
	char *pointer = strstr(dte->getResponse(), "+CNUM: ") + strlen_P((const char *)F("+CNUM: "));
	char *str = strtok(pointer, ",\r\n");
	unsigned char i = 0;
	for (i = 0; i < 5 && str != NULL; i++) {
		if(i == 0) strcpy(subscriberNumber.characterSet, str);
		if(i == 1) strcpy(subscriberNumber.number, str);
		if(i == 2) subscriberNumber.type = atoi(str);
		if(i == 3) subscriberNumber.speed = str[0] - '0';
		if(i == 4) subscriberNumber.service = str[0] - '0';
		str = strtok(NULL, ",\r\n");
	}
	strcpy(subscriberNumber.characterSet, strtok(subscriberNumber.characterSet, "\""));
	strcpy(subscriberNumber.number, strtok(subscriberNumber.number, "\""));
	if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
	this->subscriberNumber = subscriberNumber;
	return true;
}

bool GSM::atClock(void) {
	dte->clearReceivedBuffer();
	dte->ATCommand(F("AT+CCLK?\r"));
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(F("AT+CCLK?\r\r\n"))) return false;
	}
	if(!dte->ATResponseContain(F("+CCLK: "))) return false;
	char *pointer = strstr(dte->getResponse(), "+CCLK: ") + strlen_P((const char *)F("+CCLK: "));
	char *str = strtok(pointer, "\"\r\n");
	if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
	strcpy(clock.string, str);
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
	char buffer[strlen_P((const char *)F("AT+CCLK=\"\"\r\r\n")) + strlen(timestamp) + 1];

	sprintf(buffer, "AT+CCLK=\"%s\"\r", timestamp);

	dte->clearReceivedBuffer();
	dte->ATCommand(buffer);
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(strcat(buffer, "\r\n"))) return false;
	}
	if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
	return true;
}

bool GSM::atBatteryCharge(void) {
	bool batteryChargeStatus = false;
	unsigned char batteryCapacityLevel = 0;
	float voltage = 0.0f;

	dte->clearReceivedBuffer();
  dte->ATCommand(F("AT+CBC\r"));
	if(dte->isEcho()) {
		if(!dte->ATResponseEqual(F("AT+CBC\r\r\n"))) return false;
	}
	if(!dte->ATResponseContain(F("+CBC: "))) return false;
	char *pointer = strstr(dte->getResponse(), "+CBC: ") + strlen_P((const char *)F("+CBC: "));
	char *str = strtok(pointer, ",\r\n");
	unsigned char i = 0;
	for (i = 0; i < 3 && str != NULL; i++) {
		if(i == 0) batteryChargeStatus = (str[0] == '1')?true:false;
		if(i == 1) batteryCapacityLevel = atoi(str);
		if(i == 2) voltage = (float)atoi(str) * 0.001;
		str = strtok(NULL, ",\r\n");
	}
	if(!dte->ATResponseEqual(F("OK\r\n"))) return false;
	batteryStatus.charge = batteryChargeStatus;
	batteryStatus.capacityLevel = batteryCapacityLevel;
	batteryStatus.voltage = voltage;
	return true;
}
