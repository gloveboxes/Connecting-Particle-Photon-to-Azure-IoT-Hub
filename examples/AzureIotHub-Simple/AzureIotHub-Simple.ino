// This #include statement was automatically added by the Particle IDE.
#include "ArduinoJson.h"
#include "AzureIotHubClient.h"

#define CONNECTON_STRING "<Your Azure IoT Hub or Azure IoT Central Connection String>"

int count = 0;
int msgId = 0;
char telemetryBuffer[256];

IotHub hub(CONNECTON_STRING);

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  RGB.control(true);
  Time.zone(0);
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

char *telemetryToJson()
{
  /*  https://arduinojson.org/
    use to calculate JSON_OBJECT_SIZE https://arduinojson.org/v5/assistant/
    Have allowed for a few extra json fields that actually being used at the moment
*/
  DynamicJsonBuffer jsonBuffer(JSON_OBJECT_SIZE(12) + 200);
  JsonObject &root = jsonBuffer.createObject();

  root["deviceid"] = hub.getDeviceId();

  root["temp"] = 20 + random(-3, 3); // random temperature for sample
  root["humidity"] = 70 + random(-20, 20);
  root["pressure"] = 1080 + random(-100, 100);
  root["light"] = 50 + random(-50, 50);

  root["geo"] = "Sydney";
  root["utc"] = Time.format(Time.now(), TIME_FORMAT_ISO8601_FULL).c_str();
  root["mem"] = System.freeMemory();
  root["id"] = ++msgId;
  root["schema"] = 1;

  root.printTo(telemetryBuffer, sizeof(telemetryBuffer));

  return telemetryBuffer;
}
