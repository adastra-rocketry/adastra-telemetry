/*
  debug_LED.h - Library for accessing the RGB LED on Arduino nano 33 BLE
*/
#ifndef Debug_LED_h
#define Debug_LED_h

#include "Arduino.h"

class Debug_LED
{
  public:
    Debug_LED(int pin1, int pin2, int pin3);
    void setColor(bool r, bool g, bool b);
    void off();
  private:
    int _pin1, _pin2, _pin3;
};

#endif
