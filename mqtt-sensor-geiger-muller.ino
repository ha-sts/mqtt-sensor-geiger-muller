#include "config.h"
#include "Ticker.h"
#include "sensor-geiger.h"

//RadiationWatch radiationWatch(GM_PULSE, GM_NOISE);
Ticker msgTicker;
Ticker ledTicker;
SensorGeiger sensorGeiger(GM_PULSE);

void setup() {
  pinMode(ESP8266_LED, OUTPUT);
  Serial.begin(115200);
  //radiationWatch.setup();
  //radiationWatch.registerRadiationCallback(&onRadiation);
  msgTicker.attach(6, onMqttTimer);

  //geigerSensorSetup();
  sensorGeiger.setup();
}

void loop() {
  //geigerSensorLoop();
  sensorGeiger.loop();
}

void setLedOn() {
  digitalWrite(ESP8266_LED, LOW);
}

void setLedOff() {
  digitalWrite(ESP8266_LED, HIGH);
}

void onRadiation() {
  setLedOn();
  ledTicker.once_ms(100, setLedOff);
}

void onMqttTimer() {
  // Send an MQTT message with the latest counts
  Serial.println("A wild gamma ray appeared");
  //Serial.print(uSvh());
  Serial.print(sensorGeiger.uSvh());
  Serial.print(" uSv/h +/- ");
  //Serial.println(uSvhError());
  Serial.println(sensorGeiger.uSvhError());
  //Serial.print(radiationCount());
  //Serial.print(" - ");
  //Serial.print(currentRadiationCount());
  //Serial.print(" - ");
  //Serial.println(currentNoiseCount());
}
