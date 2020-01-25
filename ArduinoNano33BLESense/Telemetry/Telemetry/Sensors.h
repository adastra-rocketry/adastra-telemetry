/*
  Sensors.h
*/
#ifndef Sensors_h
#define Sensors_h

#include "Arduino.h"
#include "Sound.h"
#include "State.h"

class Sensors
{
  public:
    Sensors();
    void init(State& initState);
    float readPressure();
    float readTemperature();
    void readAcceleration(float &acc_x, float &acc_y, float &acc_z);
  private:
    void initBarometer();
    void initAccelerometer();
    void initTemperatureSensor();
    State state;
};

#endif
