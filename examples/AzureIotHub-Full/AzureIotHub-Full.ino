// This #include statement was automatically added by the Particle IDE.
#include "ArduinoJson.h"
#include "AzureIotHubClient.h"

#define CONNECTION_STRING "<Your Azure IoT Hub or Azure IoT Central Connection String>"

int count = 0;
int msgId = 0;
char telemetryBuffer[256];
#define FAN_PIN D2

// define callback signature
void callbackCloud2Device(char *topic, byte *payload, unsigned int length);
int callbackDirectMethod(char *method, byte *payload, unsigned int length);

IotHub hub(CONNECTION_STRING, callbackCloud2Device, callbackDirectMethod);

void setup()
{
  WiFi.selectAntenna(ANT_INTERNAL);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, HIGH);
  RGB.control(true);
  Time.zone(0);
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);

  //loop returns true if connected to Azure IoT Hub
  if (hub.loop())
  {
    if (count++ % 25 == 0)  // slow down the publish rate to every 25 loops
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
int callbackDirectMethod(char *method, byte *payload, unsigned int payloadLength)
{
  toLowerCase(method, strlen(method));

  if (strcmp(method, "lighton") == 0)
  {
    RGB.color(255, 255, 0);
  }
  else if (strcmp(method, "lightoff") == 0)
  {
    RGB.color(0, 0, 0);
  }
  else if (strcmp(method, "fanon") == 0)
  {
    int speed = jsonGetValue(payload, payloadLength, "speed");
    if (speed == -1) {return 500;}
    // input between 0 and 100, inverted, and scale pwm between 0 and 255 duty cycle
    speed = 100 - constrain(speed, 0, 100);
    speed = map(speed, 0, 100, 0, 255);
    analogWrite(FAN_PIN, speed, 1000);
  }
  else if (strcmp(method, "fanoff") == 0)
  {
    digitalWrite(FAN_PIN, HIGH);
  }
  else
  {
    return 400;
  }

  return 200;
}

int jsonGetValue(byte* payload, int payloadLength, char * key){
  char pl[payloadLength + 1];
  memcpy(pl, payload, payloadLength);
  pl[payloadLength] = NULL;

  //only expecting json with a single element - allow a little extra padding
  // https://arduinojson.org/v5/assistant/

  const int capacity = JSON_OBJECT_SIZE(2) + 30;
  DynamicJsonBuffer jsonBuffer(capacity);
  JsonObject& root = jsonBuffer.parseObject(pl);

  if (root.success()) {
    int speed = root[key];
    return speed;
  } else {
    return -1;
  }
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
  DynamicJsonBuffer jsonBuffer(JSON_OBJECT_SIZE(12) + 250);
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
