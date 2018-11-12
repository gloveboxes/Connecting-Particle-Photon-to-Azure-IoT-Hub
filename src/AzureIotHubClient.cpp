#include "AzureIotHubClient.h"

void IotHub::startConnection()
{
  waitFor(WiFi.ready, 5000);

  if (WiFi.ready())
  {
    client->enableTls(letencryptCaPem, strlen(letencryptCaPem) + 1);
    client->connect(this->device.deviceId, this->endPoint, this->sas.token);
    client->subscribe(this->topic.messageSubscribe);
    client->subscribe(this->topic.directMethodSubscribe);

    this->wasConnected = true;
  }
}

void IotHub::stopConnection()
{
  client->unsubscribe(this->topic.messageSubscribe);
  client->unsubscribe(this->topic.directMethodSubscribe);
  client->disconnect();

  this->wasConnected = false;
}

bool IotHub::connected()
{
  return WiFi.ready() && client->isConnected();
}

bool IotHub::loop()
{
  if (this->wasConnected)
  {
    if (connected())
    {
      // generareSas returns true if new sas token created
      // so stop existing mqtt connection and reopen using new sas token
      if (generateSasToken())
      {
        stopConnection();
        startConnection();
      }
      client->loop(); // check if a new message from iot hub
      return true;
    }
    else
    {
      stopConnection();
      return false;
    }
  }
  else
  {
    if (WiFi.ready())
    {
      Particle.syncTime();
      waitUntil(Particle.syncTimeDone);

      generateSasToken();
      startConnection();
      client->loop(); // check if new message from iot hub

      return true;
    }
    else
    {
      return false;
    }
  }
}

bool IotHub::publish(String msg)
{
  if (connected())
  {
    client->publish(this->topic.messagePublish, msg);
  }
}

void IotHub::directMethodReponse(char *rid, int status)
{
  String response = topic.directMethodResponse + String(status) + "/?$rid=" + String(rid);
  client->publish(response, "");
}

void IotHub::tokeniseConnectionString(char *cs)
{
  char *value;
  char *pch = strtok(cs, ";");

  while (pch != NULL)
  {
    value = getValue(pch, "HostName");
    if (NULL != value)
    {
      this->device.host = value;
    }
    value = getValue(pch, "DeviceId");
    if (NULL != value)
    {
      this->device.deviceId = value;
    }
    value = getValue(pch, "SharedAccessKey");
    if (NULL != value)
    {
      this->device.deviceKey = value;
    }
    pch = strtok(NULL, ";");
  }
}

char *IotHub::getValue(char *token, char *key)
{
  int valuelen;
  int keyLen = strlen(key) + 1; // plus 1 for = char

  if (NULL == strstr(token, key))
  {
    return NULL;
  }

  valuelen = strlen(token + keyLen);
  char *arr = (char *)malloc(valuelen + 1);
  strcpy(arr, token + keyLen);
  return arr;
}
