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
    void readGyroscope(float &g_x, float &g_y, float &g_z);
  private:
    void initBarometer();
    void initIMU();
    void initTemperatureSensor();
    State state;
};

#endif
