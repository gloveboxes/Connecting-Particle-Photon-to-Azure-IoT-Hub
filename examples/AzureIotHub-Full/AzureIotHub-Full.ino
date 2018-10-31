// This #include statement was automatically added by the Particle IDE.
#include "ArduinoJson.h"
#include "AzureIotHubClient.h"

#define HOST "your.azure-devices.net"
#define DEVICE "azure-iot-hub-device-id"
#define DEVICE_KEY "azure-iot-hub-device-key"

int count = 0;
int msgId = 0;
char telemetryBuffer[256];

// define callback signiture
void callbackCloud2Device(char *topic, byte *payload, unsigned int length);
int callbackDirectMethod(char *method, byte *payload, unsigned int length);

IotHub hub(HOST, DEVICE, DEVICE_KEY, callbackCloud2Device, callbackDirectMethod);

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  RGB.control(true);

  Particle.syncTime();
  waitUntil(Particle.syncTimeDone);
  Time.zone(0); // utc
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);

  //loop returns true if connected to Azure IoT Hub
  if (hub.loop())
  {
    if (count++ % 25 == 0)
    {
      Serial.printf("msg id: %d\n", msgId);
      hub.publish(telemetryToJson());
    }
  }

  delay(20); // allow for a short blink before turning led off
  digitalWrite(LED_BUILTIN, LOW);
  delay(180);
}

// process cloud to device message
void callbackCloud2Device(char *topic, byte *payload, unsigned int length)
{
  char* msg = (char*)payload;
  toLowerCase(msg, length);

  if (strncmp(msg, "red", length) == 0)
  {
    RGB.color(255, 0, 0);
  }
  else if (strncmp(msg, "green", length) == 0)
  {
    RGB.color(0, 255, 0);
  }
  else if (strncmp(msg, "blue", length) == 0)
  {
    RGB.color(0, 0, 255);
  }
}

/*
    process direct message
    https://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
    Return codes by convention: 200 = sucess, 400 = invalid request, 500 = issue processing request
    */
int callbackDirectMethod(char *method, byte *payload, unsigned int length)
{
  if (strlen(method) > 10)
  {
    return 400;
  }

  toLowerCase(method, strlen(method));

  if (strcmp(method, "on") == 0)
  {
    RGB.color(255, 255, 0);
  }
  else if (strcmp(method, "off") == 0)
  {
    RGB.color(0, 0, 0);
  }
  else
  {
    return 400;
  }

  return 200;
}

void toLowerCase(char* s, size_t length){
  for (size_t i = 0; i < length; i++)
  {
    s[i] = tolower(s[i]);
  }
}

char *telemetryToJson()
{
  /*  https://arduinojson.org/
    use to calculate JSON_OBJECT_SIZE https://arduinojson.org/v5/assistant/
    Have allowed for a few extra json fields that actually being used at the moment
*/
  DynamicJsonBuffer jsonBuffer(JSON_OBJECT_SIZE(12) + 200);
  JsonObject &root = jsonBuffer.createObject();

  root["DeviceId"] = DEVICE;

  root["Celsius"] = 20 + random(-3, 3); // random temperature for sample
  root["Humidity"] = 70 + random(-20, 90);
  root["hPa"] = 1080 + random(-100, 100);
  root["Light"] = 50 + random(-50, 50);

  root["Geo"] = "Sydney";
  root["Utc"] = Time.format(Time.now(), TIME_FORMAT_ISO8601_FULL).c_str();
  root["Mem"] = System.freeMemory();
  root["Id"] = ++msgId;
  root["Schema"] = 1;

  root.printTo(telemetryBuffer, sizeof(telemetryBuffer));

  return telemetryBuffer;
}
