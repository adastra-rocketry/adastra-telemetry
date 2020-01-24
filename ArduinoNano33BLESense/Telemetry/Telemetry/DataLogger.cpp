#include "Arduino.h"
#include "DataLogger.h"
#include "Settings.h"
#include "State.h"

#include <SPI.h>
#include <SD.h>


DataLogger::DataLogger() {

}

void DataLogger::init() {
  if (!SD.begin(SD_CARD_PIN)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    SDavailable = false;
  }
  if(SDavailable) {
    createNewFile();
    createCsvHeadings();
  }
}

void DataLogger::saveValue(DataPoint& newValue) {

  _storage[_counter] = newValue;
  _counter++;

  if(SDavailable) {
    createCsvLine(newValue);
  }
}

bool DataLogger::hasSpaceLeft() {
  return _counter < STORAGE_SIZE;
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

void DataLogger::createNewFile() {
  int n = 0;
  snprintf(filename, sizeof(filename), "data%03d.csv", n);
  while(SD.exists(filename)) {
    n++;
    snprintf(filename, sizeof(filename), "data%03d.csv", n);
  }
  dataFile = SD.open(filename, FILE_WRITE);
}

void DataLogger::createCsvHeadings() {
  String headings = "state;timestamp;pressure;temperature;accX;accY;accZ;pressureDelta;pressureDeltaMid;altitude";
  if (dataFile) {
    dataFile.println(headings);
    dataFile.flush();
  }
}

void DataLogger::createCsvLine(DataPoint& dataPoint) {
  char dataString [400];
  snprintf ( dataString, 400, "%u,%lu,%f,%f,%f,%f,%f,%f,%f,%f", 
    dataPoint.State, 
    dataPoint.Timestamp, 
    dataPoint.Pressure, 
    dataPoint.Temperature, 
    dataPoint.Acc_X, 
    dataPoint.Acc_Y, 
    dataPoint.Acc_Z,
    dataPoint.PressureDelta,
    dataPoint.PressureDeltaMid,
    dataPoint.Altitude
  );
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.flush();
  }  
}

void DataLogger::empty() {
  if(DEBUG) Serial.println("clearing");
  if(SDavailable) {
    dataFile.close();
    createNewFile();
    createCsvHeadings();
  }
  _counter = 0;
  _positionCounter = 0;
}
