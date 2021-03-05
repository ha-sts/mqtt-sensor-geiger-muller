#ifndef SENSOR_GEIGER_H
#define SENSOR_GEIGER_H

#include "Arduino.h"

#define RW_HISTORY_LENGTH 200
#define RW_HISTORY_UNIT 6
#define RW_PROCESS_PERIOD 160
#define RW_KALPHA 53.032

class SensorGeiger {
  public:
    SensorGeiger(byte pulsePinNumber);

    void setup();
    void loop();
    double cpm();
    double cpmError();
    double uSvh();
    double uSvhError();

  protected:
    unsigned int _countHistory[RW_HISTORY_LENGTH];
    unsigned long previousTime;
    unsigned long previousHistoryTime;
    unsigned long _count;
    byte historyIndex;
    byte historyLength;
    static int volatile _radiationCount;
    byte pulsePin;

    void (*_radiationCallback)(void);
    void setupInterrupt();
    static void _onRadiationHandler();
};

#endif // SENSOR_GEIGER_H
