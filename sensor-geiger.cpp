#include "sensor-geiger.h"

// Variables
//unsigned int _countHistory[RW_HISTORY_LENGTH];
//unsigned long previousTime = 0;
//unsigned long previousHistoryTime = 0;
//unsigned long _count = 0;
//unsigned char historyIndex = 0;
//unsigned char historyLength = 0;
//static int volatile _radiationCount = 0;
//static int volatile _noiseCount = 0;

int volatile SensorGeiger::_radiationCount = 0;

// Interrupts
void ICACHE_RAM_ATTR SensorGeiger::_onRadiationHandler()
{
  _radiationCount++;
}

//void ICACHE_RAM_ATTR _onNoiseHandler()
//{
//  _noiseCount++;
//}

// Functions

SensorGeiger::SensorGeiger(byte pulsePinNumber) {
  pulsePin = pulsePinNumber;
}

//void geigerSensorSetup() {
void SensorGeiger::setup() {
  pinMode(pulsePin, INPUT_PULLUP);
  //pinMode(GM_NOISE, INPUT_PULLUP);
  for(int i = 0; i < RW_HISTORY_LENGTH; i++)
    _countHistory[i] = 0;
  _count = 0;
  historyIndex = 0;
  historyLength = 0;
  // Init measurement time.
  previousTime = millis();
  previousHistoryTime = millis();
  // Attach interrupt handlers.
  attachInterrupt(digitalPinToInterrupt(pulsePin), _onRadiationHandler, FALLING);
  //attachInterrupt(digitalPinToInterrupt(GM_NOISE), _onNoiseHandler, RISING);
}

//void geigerSensorLoop() {
void SensorGeiger::loop() {
  //radiationWatch.loop();

  // Process radiation dose if the process period has elapsed.
  unsigned long currentTime = millis();
  if(currentTime - previousTime >= RW_PROCESS_PERIOD) {
    noInterrupts();
    int currentCount = _radiationCount;
    //int currentNoiseCount = _noiseCount;
    _radiationCount = 0;
    //_noiseCount = 0;
    interrupts();
    //if(currentNoiseCount == 0) {
      // Store count log.
      _countHistory[historyIndex] += currentCount;
      // Add number of counts.
      _count += currentCount;
    //}
    // Shift an array for counting log for each 6 seconds.
    if(currentTime - previousHistoryTime >= RW_HISTORY_UNIT * 1000) {
      previousHistoryTime += (unsigned long)(RW_HISTORY_UNIT * 1000);
      historyIndex = (historyIndex + 1) % RW_HISTORY_LENGTH;
      if(historyLength < (RW_HISTORY_LENGTH-1)) {
        // Since, we overwrite the oldest value in the history,
        // the effective maximum length is HISTORY_LENGTH-1
        historyLength++;
      }
      _count -= _countHistory[historyIndex];
      _countHistory[historyIndex] = 0;
    }
    // Save time of current process period
    previousTime = currentTime;
    // Enable the callbacks.
    //if(_noiseCallback && currentNoiseCount > 0) {
    //  _noiseCallback();
    //}
    //if(_radiationCallback && currentCount > 0) {
    //  _radiationCallback();
    //}
    //if(currentCount > 0) {
    //  onRadiation();
    //}
    //Serial.print(currentCount);
    //Serial.print(" - ");
    //Serial.println(currentNoiseCount);
  }
}

//unsigned long integrationTime()
//{
//  return (historyLength * RW_HISTORY_UNIT * 1000UL + previousTime - previousHistoryTime);
//}

//int currentRadiationCount() {
//  noInterrupts();
//  int currentCount = _radiationCount;
//  interrupts();
//  return currentCount;
//}

//int currentNoiseCount() {
//  noInterrupts();
//  int currentCount = _noiseCount;
//  interrupts();
//  return currentCount;
//}

//unsigned long radiationCount() {
//  return _count;
//}

double SensorGeiger::cpm()
{
  // cpm = uSv x alpha
  double min = (historyLength * RW_HISTORY_UNIT * 1000UL + previousTime - previousHistoryTime) / 60000.0;
  return (min > 0) ? _count / min : 0;
}

double SensorGeiger::uSvh()
{
  return cpm() / RW_KALPHA;
}

double SensorGeiger::uSvhError()
{
  double min = (historyLength * RW_HISTORY_UNIT * 1000UL + previousTime - previousHistoryTime) / 60000.0;
  return (min > 0) ? sqrt(_count) / min / RW_KALPHA : 0;
}
