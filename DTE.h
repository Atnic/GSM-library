/*
 *
 */
#ifndef DTE_h
#define DTE_h

#include <Arduino.h>
#include <SoftwareSerial.h>

// Library Interface Description
class DTE
{
private:
	HardwareSerial *hardwareSerial = NULL;
	SoftwareSerial *softwareSerial = NULL;
	int powerPin;
	bool debug = false;
	char response[50];
	bool echo = true;
	bool pdu = true;
	bool powerDown = true;

protected:
	void debugPrint(const char message[], bool returnChar = false);
	void debugPrint(const __FlashStringHelper *message, bool returnChar = false);

public:
	DTE(HardwareSerial &hardwareSerial, int pinPower, bool debug = false);
	DTE(SoftwareSerial &softwareSerial, int pinPower, bool debug = false);

  int available(void);
	bool isListening(void);
	bool listen(void);

	void togglePower(void);

  void clearReceivedBuffer(void);

	bool AT(void);
	bool ATCommand(const char at[]);
	bool ATCommand(const __FlashStringHelper *at);

	bool ATResponse(unsigned long timeout = 500);
	bool ATResponseEqual(const char expected[], unsigned long timeout = 500);
	bool ATResponseEqual(const __FlashStringHelper *expected, unsigned long timeout = 500);
	bool ATResponseContain(const char expected[], unsigned long timeout = 500);
	bool ATResponseContain(const __FlashStringHelper *expected, unsigned long timeout = 500);
	bool ATResponseOk(unsigned long timeout = 500);

	bool isResponseEqual(const char expected[]);
	bool isResponseEqual(const __FlashStringHelper *expected);
	bool isResponseContain(const char expected[]);
	bool isResponseContain(const __FlashStringHelper *expected);
	bool isResponseOk(void);

	void setEcho(bool echo) { this->echo = echo; }

	const char *getResponse(void) { return response; }
	bool isEcho(void) { return echo; }
	bool isPowerDown(void) { return powerDown; };

	bool powerReset(void);
};

#endif
