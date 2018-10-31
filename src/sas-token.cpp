#include "sas-token.h"

int SasToken::urlEncode(char *dest, char *msg)
{
  const char *hex = "0123456789abcdef";
  char *startPtr = dest;

  while (*msg != '\0')
  {
    if (('a' <= *msg && *msg <= 'z') || ('A' <= *msg && *msg <= 'Z') || ('0' <= *msg && *msg <= '9'))
    {
      *dest++ = *msg;
    }
    else
    {
      *dest++ = '%';
      *dest++ = hex[*msg >> 4];
      *dest++ = hex[*msg & 15];
    }
    msg++;
  }
  *dest = '\0';
  return dest - startPtr;
}

void SasToken::createSas(char *key)
{
  sasExpiryTime = currentEpochTime() + sasExpiryPeriodInSeconds;

  int keyLength = strlen(key);

  int decodedKeyLength = base64_dec_len(key, keyLength);
  memset(buff, 0, decodedKeyLength + 1); // initialised extra byte to allow for null termination

  base64_decode(buff, key, keyLength); //decode key
  Sha256.initHmac((const uint8_t *)buff, decodedKeyLength);

  int len = snprintf(buff, sizeof(buff), "%s/devices/%s\n%d", this->host, this->deviceId, sasExpiryTime);
  Sha256.print(buff);

  char *sign = (char *)Sha256.resultHmac();

  int encodedSignLen = base64_enc_len(HASH_LENGTH); // START: Get base64 of signature
  memset(buff, 0, encodedSignLen + 1);              // initialised extra byte to allow for null termination

  base64_encode(buff, sign, HASH_LENGTH);

  char *sasPointer = fullSas;
  sasPointer += snprintf(sasPointer, sizeof(fullSas), "SharedAccessSignature sr=%s/devices/%s&sig=", this->host, this->deviceId);
  sasPointer += urlEncode(sasPointer, buff);
  snprintf(sasPointer, sizeof(fullSas) - (sasPointer - fullSas), "&se=%d", sasExpiryTime);
}

void SasToken::syncTime()
{
  if (millis() - lastTimeSync > ONE_DAY_MILLIS)
  {
    Particle.syncTime();
    lastTimeSync = millis();
  }
}

// returns false if no new sas token generated
bool SasToken::generateSas()
{
  if (currentEpochTime() < sasExpiryTime)
  {
    return false;
  }
  syncTime();
  createSas(key);
  return true;
}

time_t SasToken::currentEpochTime()
{
  return tmConvert_t(Time.year(), Time.month(), Time.day(), Time.hour(), Time.minute(), Time.second());
}

time_t SasToken::tmConvert_t(int YYYY, byte MM, byte DD, byte hh, byte mm, byte ss)
{
  t.tm_year = YYYY - 1900;
  t.tm_mon = MM - 1;
  t.tm_mday = DD;
  t.tm_hour = hh;
  t.tm_min = mm;
  t.tm_sec = ss;
  t.tm_isdst = 0;

  time_t t_of_day = mktime(&t);
  return t_of_day;
}
