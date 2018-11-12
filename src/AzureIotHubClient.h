#ifndef _IOTHUB_H
#define _IOTHUB_H

#include <MQTT-TLS.h>
#include <Particle.h>
#include "SasToken.h"
// #include <string.h>

/*
Baltimore
https://github.com/Azure/azure-iot-sdk-c/blob/master/certs/certs.c
*/

#define LET_ENCRYPT_CA_PEM                                               \
  "-----BEGIN CERTIFICATE-----\r\n"                                      \
  "MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\r\n" \
  "RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\r\n" \
  "VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\r\n" \
  "DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\r\n" \
  "ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\r\n" \
  "VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\r\n" \
  "mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\r\n" \
  "IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\r\n" \
  "mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\r\n" \
  "XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\r\n" \
  "dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\r\n" \
  "jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\r\n" \
  "BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\r\n" \
  "DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\r\n" \
  "9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\r\n" \
  "jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\r\n" \
  "Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\r\n" \
  "ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\r\n" \
  "R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\r\n"                             \
  "-----END CERTIFICATE-----\r\n"

static void (*__callbackCloud2Device)(char *, uint8_t *, unsigned int);
static int (*__callbackDirectMethod)(char *, uint8_t *, unsigned int);

class IotHub;
static IotHub *__iotHubInstance;

class IotHub : SasToken
{
public:
  IotHub(const char *connectionString,
         void (*callbackCloud2Device)(char *, uint8_t *, unsigned int) = NULL,
         int (*callbackDirectMethod)(char *, uint8_t *, unsigned int) = NULL,
         int maxBufferSize = 500,
         time_t sasExpiryPeriodInSeconds = 3600) // default to 60 minute sas token expiry
  {

    if (NULL != __iotHubInstance)
    {
      return;
    }

    // because connection string gets tokenised with strtok the cs needs to be in memory
    device.connectionString = (char *)malloc(strlen(connectionString) + 1);
    strcpy(this->device.connectionString, connectionString);

    tokeniseConnectionString(device.connectionString);

    sas.expiryPeriodInSeconds = sasExpiryPeriodInSeconds;

    __callbackCloud2Device = callbackCloud2Device;
    __callbackDirectMethod = callbackDirectMethod;

    topic.messagePublish = "devices/" + String(device.deviceId) + "/messages/events/";
    topic.messageSubscribe = "devices/" + String(device.deviceId) + "/messages/devicebound/#";
    topic.directMethodSubscribe = "$iothub/methods/POST/#";
    topic.directMethodResponse = "$iothub/methods/res/";

    endPoint = String(device.host) + "/" + String(device.deviceId) + "/api-version=2016-11-14";

    client = new MQTT(device.host, 8883, processSubTopic, maxBufferSize);

    __iotHubInstance = this;
  }

  bool loop();
  bool connected();
  bool publish(String msg);
  char *getDeviceId()
  {
    return device.deviceId;
  }

private:
  void startConnection();
  void stopConnection();
  void tokeniseConnectionString(char *cs);
  char *getValue(char *token, char *key);
  void (*directMethodCallback)(char *, uint8_t *, unsigned int);
  void directMethodReponse(char *rid, int status);

  bool wasConnected = false;
  MQTT *client;
  String endPoint;

  struct topics
  {
    String messagePublish;
    String messageSubscribe;
    String directMethodSubscribe;
    String directMethodResponse;
  } topic;

  const char *letencryptCaPem = LET_ENCRYPT_CA_PEM;

  static void directMessage(char *topic, byte *payload, unsigned int payloadLength)
  {
    char *startMethodName = strstr(topic, "POST/") + 5;
    if (NULL == startMethodName)
    {
      return;
    }

    char *endMethodName = strstr(startMethodName, "/?");
    if (NULL == endMethodName)
    {
      return;
    }

    int methodNamelength = endMethodName - startMethodName;

    char methodName[methodNamelength + 1];
    memset(methodName, 0, methodNamelength + 1);
    memcpy(methodName, startMethodName, methodNamelength);

    int status = __callbackDirectMethod(methodName, payload, payloadLength);

    char *rid = strstr(topic, "rid=") + 4;
    __iotHubInstance->directMethodReponse(rid, status);
  }

  static void processSubTopic(char *topic, byte *payload, unsigned int payloadLength)
  {
    char *p1;

    p1 = strstr(topic, "$iothub/methods/POST/"); // handle request response direct message
    if (p1 == topic && NULL != __callbackDirectMethod)
    {
      directMessage(topic, payload, payloadLength);
      return;
    }

    p1 = strstr(topic, "devices/"); // handle cloud to device messages
    if (p1 == topic && NULL != __callbackCloud2Device)
    {
      __callbackCloud2Device(topic, payload, payloadLength);
      return;
    }
  }
};

#endif // _IOTHUB_H
