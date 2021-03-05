#include "sensor-geiger.h"

int volatile SensorGeiger::_radiationCount = 0;

// Interrupts
void ICACHE_RAM_ATTR SensorGeiger::_onRadiationHandler()
{
  _radiationCount++;
}

// Functions
SensorGeiger::SensorGeiger(byte pulsePinNumber) {
  pulsePin = pulsePinNumber;
}

//void geigerSensorSetup() {
void SensorGeiger::setup() {
  pinMode(pulsePin, INPUT_PULLUP);
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
}

//void geigerSensorLoop() {
void SensorGeiger::loop() {
  // Process radiation dose if the process period has elapsed.
  unsigned long currentTime = millis();
  if(currentTime - previousTime >= RW_PROCESS_PERIOD) {
    noInterrupts();
    int currentCount = _radiationCount;
    _radiationCount = 0;
    interrupts();
    
    // Store count log.
    _countHistory[historyIndex] += currentCount;
    // Add number of counts.
    _count += currentCount;
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
    //if(_radiationCallback && currentCount > 0) {
    //  _radiationCallback();
    //}
  }
}

double SensorGeiger::cpm() {
  // cpm = uSv x alpha
  // integrationTime = (historyLength * RW_HISTORY_UNIT * 1000UL + previousTime - previousHistoryTime)
  double min = (historyLength * RW_HISTORY_UNIT * 1000UL + previousTime - previousHistoryTime) / 60000.0;
  return (min > 0) ? _count / min : 0;
}

double SensorGeiger::cpmError() {
  double min = (historyLength * RW_HISTORY_UNIT * 1000UL + previousTime - previousHistoryTime) / 60000.0;
  return (min > 0) ? sqrt(_count) / min : 0;
}

double SensorGeiger::uSvh() {
  return cpm() / RW_KALPHA;
}

double SensorGeiger::uSvhError() {
  return cpmError() / RW_KALPHA;
}
