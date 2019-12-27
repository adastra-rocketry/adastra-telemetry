/*
  State.h
*/
#ifndef State_h
#define State_h

#include "Arduino.h"
#include "DataLogger.h"
#include "Settings.h"

class State
{
  public:
    State();
    void init();
    Vehicle_State vehicleState = Vehicle_State::Idle;
    DataPoint currentDataPoint;
    void createDataPoint(float pressure, float temperature, float acc_x, float acc_y, float acc_z);
    DataLogger logger{};
    
  private:
    void processDataPoint(DataPoint& point);
    int _headIndex = 0;
    float _lastPressures[RING_BUFFER_SIZE]; // RING_BUFFER_SIZE from settings
    float _lastPressureDeltas[RING_BUFFER_SIZE];
};

#endif
