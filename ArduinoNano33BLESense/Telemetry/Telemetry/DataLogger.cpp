#include "Arduino.h"
#include "DataLogger.h"
#include "Settings.h"
#include "State.h"

#include <SPI.h>
#include <SD.h>


DataLogger::DataLogger() {

}

void DataLogger::init(Sound& sound) {
  if (!SD.begin(SD_CARD_PIN)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    SDavailable = false;
    sound.playError(3);
  }
  if(SDavailable) {
    createNewFile();
    createCsvHeadings();
  }
}

void DataLogger::saveValue(DataPoint& newValue) {
  _counter++;

  if(SDavailable) {
    createCsvLine(newValue);
  }
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
  String headings = "state;timestamp;pressure;temperature;accX;accY;accZ;pressureDelta;pressureDeltaMid;altitude;KalmanPressure;KalmanAltitude";
  if (dataFile) {
    dataFile.println(headings);
    dataFile.flush();
  }
}

void DataLogger::createCsvLine(DataPoint& dataPoint) {
  char dataString [400];
  snprintf ( dataString, 400, "%u;%lu;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f", 
    dataPoint.State, 
    dataPoint.Timestamp, 
    dataPoint.Pressure, 
    dataPoint.Temperature, 
    dataPoint.Acc_X, 
    dataPoint.Acc_Y, 
    dataPoint.Acc_Z,
    dataPoint.PressureDelta,
    dataPoint.KalmanPressureDelta,
    dataPoint.Altitude,
    dataPoint.KalmanPressure,
    dataPoint.KalmanAltitude,
    dataPoint.KalmanTemperature
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
}
