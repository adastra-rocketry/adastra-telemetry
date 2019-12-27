/*
  State.h
*/
#ifndef State_h
#define State_h

#include "Arduino.h"

enum Vehicle_State {
  Idle = 0,
  LaunchIdle = 1,
  Ascending = 2,
  Descending = 3,
  Landed = 4
};


class State
{
  public:
    State();
    Vehicle_State vehicleState = Vehicle_State::Idle;

};

#endif
