/*
  DataLogger.h - Library for storing and retrieving sensor data
*/
#ifndef DataLogger_h
#define DataLogger_h

#include "Arduino.h"

enum Vehicle_State {
  Idle = 0,
  LaunchIdle = 1,
  Ascending = 2,
  Descending = 3,
  Landed = 4
};

struct DataPoint {
  Vehicle_State State;
  unsigned long Timestamp;
  float Pressure;
  float Temperature;
  float Acc_X;
  float Acc_Y;
  float Acc_Z;
};

class DataLogger
{
  public:
    DataLogger();
    bool hasSpaceLeft();
    void saveValue(DataPoint& newValue);
    void empty();
    DataPoint getNextEntry();
    bool hasNextEntry();
    int getCounter();
  private:
    DataPoint _storage[1024];
    int _counter = 0;
    int _positionCounter = 0;
};

#endif
