#include "State.h"
#include "Arduino.h"
#include "DataLogger.h"
#include "Settings.h"

#include <SimpleKalmanFilter.h>

State::State() {
  
}

void State::init() {
  logger.init(sound);
}

void State::reset() {
  
  logger.reset();
  vehicleState = Vehicle_State::Idle;
  heighestAltitude = 0.0;
  altitudeKalmanFilter = SimpleKalmanFilter(1, 1, 1);
}

DataPoint State::createDataPoint(float pressure, float temperature, float acc_x, float acc_y, float acc_z, float g_x, float g_y, float g_z) {
  DataPoint newItem = {vehicleState, millis(), pressure, temperature, acc_x, acc_y, acc_z, g_x, g_y, g_z};
  processDataPoint(newItem);

  currentDataPoint = newItem;
  return newItem;
}

void State::saveDataPoint(DataPoint& state) {
  // test if landed in the future too: //  && vehicleState != Vehicle_State::Landed
  if(vehicleState != Vehicle_State::Idle) {
    logger.saveValue(state);
  }
}

void State::saveCurrentDataPoint() {
  saveDataPoint(currentDataPoint);
}

void State::updateFlightState() {
  if(vehicleState == Vehicle_State::LaunchIdle && currentDataPoint.Acc_X > 1) {
    vehicleState = Vehicle_State::Ascending;
    sound.playSound(400, 500);
  }
  else if(vehicleState == Vehicle_State::Ascending && currentDataPoint.KalmanAltitude < heighestAltitude - 2) {
    vehicleState = Vehicle_State::Descending;
    sound.playSound(400, 500);
  }
  else if(vehicleState == Vehicle_State::Descending && currentDataPoint.KalmanAltitude < LaunchAltitude + 5) {
    vehicleState = Vehicle_State::Landed;
    sound.playSound(400, 500);
  }
  
}

void State::processDataPoint(DataPoint& point) {
    float pressureDelta = lastPressure - point.Pressure;
    point.PressureDelta = pressureDelta;
    
    point.KalmanPressureDelta = pressureDeltaKalmanFilter.updateEstimate(point.PressureDelta);
    point.KalmanPressure = pressureKalmanFilter.updateEstimate(point.Pressure);
    point.KalmanTemperature = temperatureKalmanFilter.updateEstimate(point.Temperature);

    point.Altitude = calculateAltitude(LaunchAltitude, PressureNN, point.Pressure * 10, point.Temperature);
    point.KalmanAltitude = altitudeKalmanFilter.updateEstimate(point.Altitude);
    if(point.KalmanAltitude > heighestAltitude) heighestAltitude = point.KalmanAltitude;
    lastPressure = point.Pressure;
}

float State::calculateAltitude(float launchAltitude, float launchPressure, float P, float T){
  float P0 = readP0(launchAltitude, launchPressure);
  return ( ( ( pow( ( P0 / P ), ( 1/5.257 ) ) ) - 1 ) * ( T + 273.15 ) ) / 0.0065;
}

float State::readP0(float myAltitude, float abs_Pressure) {
  float p0 = abs_Pressure / pow((1.0 - ( myAltitude / 44330.0 )), 5.255);
  return p0;
}
