/*
*
*/
#ifndef TCPIP_h
#define TCPIP_h

#include <Arduino.h>
#include <DTE.h>

class TCPIP
{
private:
  DTE *dte;
  bool multiIPConnection = false;
  unsigned char connectionStatus = 0;

  unsigned char connectionStatusIndex(const char *status);

public:
  TCPIP(DTE &dte);

  bool atStartUpMultiIPConnection(void);
  bool atStartUpMultiIPConnection(bool multi);

  bool atStartUpTCPUDPConnection(const char mode[], const char host[], unsigned int port);
  bool atStartUpTCPUDPConnection(unsigned char n, const char mode[], const char host[], unsigned int port);

  bool atDeactiveGPRSPDPContext(void);

  bool atQueryCurrentConnectionStatus(void);

  bool isMultiIPConnection(void) { return multiIPConnection; }
};

#endif
