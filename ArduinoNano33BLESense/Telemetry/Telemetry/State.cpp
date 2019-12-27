#include "State.h"
#include "Arduino.h"
#include "DataLogger.h"
#include "Settings.h"

State::State() {
  
}

void State::init() {
  
}

void State::createDataPoint(float pressure, float temperature, float acc_x, float acc_y, float acc_z) {
  DataPoint newItem = {vehicleState, millis(), pressure, temperature, acc_x, acc_y, acc_z};
  processDataPoint(newItem);
  if(vehicleState != Vehicle_State::Idle && vehicleState != Vehicle_State::Landed && logger.hasSpaceLeft()) {
    logger.saveValue(newItem);
  }
}

void State::processDataPoint(DataPoint& point) {
    float lastPressure = _lastPressures[_headIndex];
    
    // ringbuffer to calculate state transitions
    _headIndex = (_headIndex + 1) % RING_BUFFER_SIZE;
    _lastPressures[_headIndex] = point.Pressure;
    float pressureDelta = lastPressure - point.Pressure;
    _lastPressureDeltas[_headIndex] = pressureDelta;

    float deltasSum = 0.0f;
    for(int k=0; k < RING_BUFFER_SIZE; k++) deltasSum += _lastPressureDeltas[k];
    float deltasMid = deltasSum / RING_BUFFER_SIZE;
    
    point.PressureDelta = pressureDelta;
    point.PressureDeltaMid = deltasMid;
    
    currentDataPoint = point;

}
