#ifndef DTE_h
#define DTE_h

#include <Arduino.h>
#include <SoftwareSerial.h>

struct FlowControl {
  unsigned char dce;
  bool dceOn;
  unsigned char dte;
  bool dteOn;
};

// Library Interface Description
class DTE {
 private:
  HardwareSerial *hardwareSerial;
  SoftwareSerial *softwareSerial;
  int powerPin;
  bool debug;
  char response[203];
  bool echo;
  char productSerialNumberIdentification[17];
  struct FlowControl flowControl;
  long baudrate;
  bool powerDown;

 protected:
  /**
   * Sent debugging message via Serial.
   * @param message    Debug message
   * @param returnChar true: add new line, false: no new line (default)
   */
  void debugPrint(const char message[], bool returnChar = false);
  void debugPrint(const __FlashStringHelper *message, bool returnChar = false);

 public:
  DTE(HardwareSerial &hardwareSerial, int pinPower, bool debug = false);
  DTE(SoftwareSerial &softwareSerial, int pinPower, bool debug = false);

  /**
   * Command A/
   * @return  true: If command successful, false: Otherwise
   */
  bool atReIssueLastCommand(void);

  /**
   * Command ATE
   * @param  echo true: To echo command, false: Otherwise
   * @return      true: If command successful, false: Otherwise
   */
  bool atSetCommandEchoMode(bool echo);

  /**
   * Command AT+IFC?
   * @return  true: If command successful, false: Otherwise
   */
  bool atSetLocalDataFlowControl(void);

  /**
   * Command AT+GSN
   * @return  true: If command successful, false: Otherwise
   */
  bool atRequestProductSerialNumberIdentification(void);

  /**
   * Command AT+IFC=
   * @param  dce Method used by TE receiving from TA
   *             0: No Flow Control
   *             1: Software Flow Control
   *             2: Hardware FLow Control
   * @param  dte Method used by TA receiving from TE,
   *             same as dceByDte
   * @return     true: If command successful, false: Otherwise
   */
  bool atSetLocalDataFlowControl(unsigned char dce, unsigned char dte = 0);

  /**
   * Command AT+IPR?
   * @return  true: If command successful, false: Otherwise
   */
  bool atSetFixedLocalRate(void);

  /**
   * Command AT+IPR=
   * @param  baudrate Baudrate
   * @return          true: If command successful, false: Otherwise
   */
  bool atSetFixedLocalRate(long baudrate);

  /**
   * Check received buffer
   * @return  Total received char available
   */
  int available(void);

  /**
   * If using SoftwareSerial, this method is to check whether
   * the serial is listening (active)
   * @return  true: listening, false: otherwise
   */
  bool isListening(void);

  /**
   * Listen (active) SoftwareSerial for this instance
   * @return  true: success, false: failed
   */
  bool listen(void);

  /**
   * Flush TX Buffer
   */
  void flush(void);

  /**
   * Send char
   * @param  c Char to be sent
   * @return     Total successfully sent char
   */
  size_t write(const char c);

  /**
   * Send string char array
   * @param  str String to be sent
   * @return     Total successfully sent char
   */
  size_t write(const char str[]);
  size_t write(const __FlashStringHelper *str);

  /**
   * Read bytes received from Serial buffer, and save it on buffer.
   * Warning: buffer size must be equal or smaller then length.
   * @param  buffer Buffer to be updated
   * @param  length Length of received byte that want to be get
   * @return        Total successfully read char
   */
  size_t readBytes(char buffer[], size_t length);

  /** Toggle SIM Power, On become Off, and otherwise */
  void togglePower(void);

  /** Clear received Serial buffer */
  void clearReceivedBuffer(void);

  /**
   * Command: AT
   * @return  true: if nothing is wrong, false: otherwise
   */
  bool AT(void);

  /**
   * Send AT Command
   * @param  at Command
   * @return    true: if nothing is wrong, false: otherwise
   */
  bool ATCommand(const char at[]);
  bool ATCommand(const __FlashStringHelper *at);

  /**
   * Get AT Response, this function is block call until timeout.
   * If response is received then, get it until "\\r\\n" chars
   * @param   buffer      Buffer to store string from SIM Module
   * @param   bufferSize Specified buffer size
   * @param  timeout    Timeout in millis, default: 500
   * @return          true: Response received, false: Timeout is reached
   */
  bool ATResponse(char buffer[], size_t bufferSize, unsigned long timeout = 500);

  /**
   * Get AT Response, this function is block call until timeout.
   * If response is received then, get it until "\\r\\n" chars
   * @param  timeout Timeout in millis, default: 500
   * @return       true: Response received, false: Timeout is reached
   */
  bool ATResponse(unsigned long timeout = 500);

  /**
   * Get AT Response, and check if response is equal with expected
   * @param  expected     Expected response
   * @param  timeout      Timeout in millis, default: 500
   * @return              true: If response as expected, false: Otherwise or timeout
   * @see   ATResponse()
   */
  bool ATResponseEqual(const char expected[], unsigned long timeout = 500);
  bool ATResponseEqual(const __FlashStringHelper *expected, unsigned long timeout = 500);

  /**
   * Get AT Response, and check if response is contain with expected
   * @param  expected     Expected response
   * @param  timeout      Timeout in millis, default: 500
   * @return              true: If response contain expected, false: Otherwise or timeout
   * @see   ATResponse()
   */
  bool ATResponseContain(const char expected[], unsigned long timeout = 500);
  bool ATResponseContain(const __FlashStringHelper *expected, unsigned long timeout = 500);

  /**
   * Get AT Response, and check if response is "OK"
   * @param  expected     Expected response
   * @param  timeout      Timeout in millis, default: 500
   * @return              true: If response "OK", false: Otherwise or timeout
   * @see   ATResponse()
   */
  bool ATResponseOk(unsigned long timeout = 500);

  /**
   * Check that last response is equal as expected
   * @param  expected Expected response
   * @return          true: If last response as expected, false: Otherwise or timeout
   */
  bool isResponseEqual(const char expected[]);
  bool isResponseEqual(const __FlashStringHelper *expected);

  /**
   * Check that last response is contain expected
   * @param  expected Expected response
   * @return          true: If last response contain expected, false: Otherwise or timeout
   */
  bool isResponseContain(const char expected[]);
  bool isResponseContain(const __FlashStringHelper *expected);

  /**
   * Check that last response is "OK"
   * @param  expected Expected response
   * @return          true: If last response is "OK", false: Otherwise or timeout
   */
  bool isResponseOk(void);

  /**
   * Unsolicited Result Code (URC) check, if it URC,
   * then update URC Object member value
   * @return     true: If it is URC, false: If it is not
   */
  bool unsolicitedResultCode(void);

  /**
   * Get last response
   * @return  Response
   */
  const char *getResponse(void) { return response; }

  /**
   * Is Echo is enable
   * @return  true: If enable, false: Otherwise
   */
  bool isEcho(void) { return echo; }

  /**
   * Set Echo
   * @param  echo true: Enable, false: Disable
   * @return      true: If success, false: Otherwise
   */
  bool setEcho(bool echo);

  /**
   * Get Product Serial Number Identification (IMEI)
   * @return  IMEI string
   */
  const char *getProductSerialNumberIdentification(void);

  /**
   * Get Flow Control
   * @return  FlowControl Struct
   */
  struct FlowControl getFlowControl(void);

  /**
   * Set Flow Control
   * @param  dce DCE by DTE
   * @param  dte DTE by DCE
   * @return     true: If command successful, false: Otherwise
   */
  bool setFlowControl(unsigned char dce, unsigned char dte = 0);

  /**
   * Set Flow Control Status on DCE
   * @param  on true: Send XON, false: Send XOFF
   * @return    true: If command successful, false: Otherwise
   */
  bool setFlowControlStatusDce(bool on);

  /**
   * Get current baudrate
   * @return  Baudrate
   */
  long getBaudrate(void);

  /**
   * Is SIM Module power is down
   * @return  true: If power is down, false: Otherwise
   */
  bool isPowerDown(void) { return powerDown; };

  /**
   * Reset Power SIM Module
   * @return  true
   */
  bool powerReset(void);
};

#endif
