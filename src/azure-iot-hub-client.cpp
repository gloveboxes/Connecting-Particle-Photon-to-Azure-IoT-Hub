#include "azure-iot-hub-client.h"

void IotHub::startConnection()
{
  client->enableTls(letencryptCaPem, strlen(letencryptCaPem) + 1);
  client->connect(this->deviceId, this->endPoint, this->fullSas);
  client->subscribe(this->subTopic);
  client->subscribe(this->subDirectMethod);

  this->wasConnected = true;
}

void IotHub::stopConnection()
{
  client->unsubscribe(this->subDirectMethod);
  client->unsubscribe(this->subTopic);
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
      if (generateSas())
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
      generateSas();
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

void IotHub::publish(String msg)
{
  client->publish(this->pubTopic, msg);
}

void IotHub::directMethodReponse(char *rid, int status)
{
  String response = directMethodResponse + String(status) + "/?$rid=" + String(rid);
  client->publish(response, "");
}