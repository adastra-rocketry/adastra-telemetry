/*
  State.h
*/
#ifndef State_h
#define State_h

#include "Arduino.h"
#include "DataLogger.h"
#include "Settings.h"
#include "Sound.h"
#include <SimpleKalmanFilter.h>


class State
{
  public:
    State();
    void init();
    Vehicle_State vehicleState = Vehicle_State::Idle;
    DataPoint currentDataPoint;
    void createDataPoint(float pressure, float temperature, float acc_x, float acc_y, float acc_z);
    DataLogger logger{};

    float LaunchAltitude = 145.0f;
    float PressureNN = 1027.6f;
    Sound sound{};
    
  private:
    void processDataPoint(DataPoint& point);
    float lastPressure;
    float calculateAltitude(float launchAltitude, float launchPressure, float P, float T);
    float readP0(float myAltitude, float abs_Pressure);

  /*SimpleKalmanFilter(e_mea, e_est, q);
    e_mea: Measurement Uncertainty 
    e_est: Estimation Uncertainty 
    q: Process Noise */
    SimpleKalmanFilter pressureKalmanFilter{1, 1, 1};
    SimpleKalmanFilter temperatureKalmanFilter{1, 1, 1};
    SimpleKalmanFilter pressureDeltaKalmanFilter{1, 1, 1};
    SimpleKalmanFilter altitudeKalmanFilter{1, 1, 1};
};

#endif
