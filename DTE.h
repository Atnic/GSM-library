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
	char response[205];
	bool debug = false;
	bool echo = true;
	bool powerDown = false;
	bool pdu = true;

protected:
	void debugPrint(const char message[], bool returnChar = false);
	void debugPrint(const __FlashStringHelper message[], bool returnChar = false);

public:
	DTE(HardwareSerial &hardwareSerial, int pinPower, bool debug = false);
	DTE(SoftwareSerial &softwareSerial, int pinPower, bool debug = false);

  int available(void);
	bool isListening(void);
	bool listen(void);

	void togglePower(void);

	void begin(long speed);
  void clearReceivedBuffer(void);

	bool AT(void);
	void ATCommand(const char at[]);
	void ATCommand(const __FlashStringHelper at[]);

	bool ATResponse(unsigned long timeout = 1000);
	bool ATResponseEqual(const char expected[], unsigned long timeout = 1000);
	bool ATResponseEqual(const __FlashStringHelper expected[], unsigned long timeout = 1000);
	bool ATResponseContain(const char expected[], unsigned long timeout = 1000);
	bool ATResponseContain(const __FlashStringHelper expected[], unsigned long timeout = 1000);

	bool powerReset(void);

	bool isResponse(const char expected[]);
	bool isResponse(const __FlashStringHelper expected[]);
	bool isResponseContain(const char expected[]);
	bool isResponseContain(const __FlashStringHelper expected[]);

	void setEcho(bool echo) { this->echo = echo; };
	void setPowerDown(bool powerDown) { this->powerDown = powerDown; };
	void setPDU(bool pdu) { this->pdu = pdu; };
	void setResponse(const char response[]) { strcpy(this->response, response); }

	bool isEcho(void) { return echo; };
	bool isPowerDown(void) { return powerDown; };
	bool isPDU(void) { return pdu; };
	const char *getResponse(void) { return response; };
};

#endif
