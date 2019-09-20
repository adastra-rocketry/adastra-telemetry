/*
  DataLogger.h - Library for storing and retrieving sensor data
*/
#ifndef DataLogger_h
#define DataLogger_h

#include "Arduino.h"

enum DataPoint_Type {
  Pressure,
  Temperature  
};

struct DataPoint {
  DataPoint_Type Type;
  unsigned long Timestamp;
  float Value;
};

class DataLogger
{
  public:
    DataLogger();
    bool hasSpaceLeft();
    void saveValue(DataPoint newValue);
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
