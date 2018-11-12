#ifndef _SASTOKEN_H
#define _SASTOKEN_H

#include <Base64.h>
#include "sha256.h"

#define MAX_HOST_DEVICE_LEN
#define BUFSIZE 150
#define SAS_TOKEN_SIZE 250
#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)

class SasToken
{
public:
  SasToken()
  {
    if (strlen(device.host) + strlen(device.deviceId) > 80)
    {
      Serial.println("WARNING: Combined number of characters exceeds 80.");
    }
  }

protected:
  bool generateSasToken();

  struct security
  {
    time_t expiryPeriodInSeconds = 60 * 60; // Default to 60 minutes
    time_t expiryTime = 0;
    char token[SAS_TOKEN_SIZE];
  } sas;

  struct config
  {
    char *connectionString;
    char *host;
    char *deviceId;
    char *deviceKey;
  } device;

private:
  unsigned long lastTimeSync = millis();

  char buff[BUFSIZE];
  // struct tm t;

  int urlEncode(char *dest, char *msg);
  void createSasToken(char *key);
  void syncTime();
};

#endif // _SASTOKEN
