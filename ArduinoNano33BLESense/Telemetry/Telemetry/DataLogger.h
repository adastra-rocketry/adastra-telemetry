/*
  DataLogger.h - Library for storing and retrieving sensor data
*/
#ifndef DataLogger_h
#define DataLogger_h

#include "Arduino.h"
#include "Settings.h"
#include <SD.h>
#include "Sound.h"


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
  
  float PressureDelta;
  float PressureDeltaMid;

  float Altitude;
};

class DataLogger
{
  public:
    DataLogger();
    void init(Sound& sound);
    void saveValue(DataPoint& newValue);
    void empty();
    int getCounter();
  private:
    bool SDavailable = true;
    void createNewFile();
    void createCsvHeadings();
    void createCsvLine(DataPoint& dataPoint);
    int _counter = 0;
    char filename[16];
    File dataFile;
};

#endif
