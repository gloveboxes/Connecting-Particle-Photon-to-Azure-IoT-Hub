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
    if (strlen(host) + strlen(deviceId) > 80)
    {
      Serial.println("WARNING: Combined number of characters exceeds 80.");
    }
  }

protected:
  bool generateSas();
  time_t sasExpiryPeriodInSeconds = 60 * 60; // Default to 60 minutes
  char *host;
  const char *deviceId;
  char *key;
  char fullSas[SAS_TOKEN_SIZE];

private:
  time_t sasExpiryTime = 0;

  unsigned long lastTimeSync = millis();

  char buff[BUFSIZE];
  struct tm t;

  int urlEncode(char *dest, char *msg);
  void createSas(char *key);
  time_t currentEpochTime();
  time_t tmConvert_t(int YYYY, byte MM, byte DD, byte hh, byte mm, byte ss);
  void syncTime();
};

#endif // _SASTOKEN