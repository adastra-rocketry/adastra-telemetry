#include "Arduino.h"
#include "DataLogger.h"

DataLogger::DataLogger() {
  
}

void DataLogger::saveValue(DataPoint& newValue) {
  _storage[_counter] = newValue;
  _counter++;
}

bool DataLogger::hasSpaceLeft() {
  return _counter < 1024;
}

bool DataLogger::hasNextEntry() {
  bool isAtEnd = _positionCounter >= _counter;
  if(isAtEnd) _positionCounter = 0;
  return (!isAtEnd);
}

DataPoint DataLogger::getNextEntry() {
  DataPoint point = _storage[_positionCounter];
  _positionCounter++;
  return point;
}

int DataLogger::getCounter() {
  return _counter;
}


void DataLogger::empty() {
  Serial.println("clearing");
  _counter = 0;
  _positionCounter = 0;
}
