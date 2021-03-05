#include "config.h"
#include "Ticker.h"
#include "sensor-geiger.h"

Ticker msgTicker;
Ticker ledTicker;
SensorGeiger sensorGeiger(GM_PULSE);

void setup() {
  pinMode(ESP8266_LED, OUTPUT);
  Serial.begin(115200);
  msgTicker.attach(6, onMqttTimer);

  sensorGeiger.setup();
  //radiationWatch.registerRadiationCallback(&onRadiation);
}

void loop() {
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
  Serial.print(sensorGeiger.uSvh());
  Serial.print(" uSv/h +/- ");
  Serial.println(sensorGeiger.uSvhError());
  Serial.print("CPM: ");
  Serial.print(sensorGeiger.cpm());
  Serial.print("   CPM Error: ");
  Serial.println(sensorGeiger.cpmError());
}
