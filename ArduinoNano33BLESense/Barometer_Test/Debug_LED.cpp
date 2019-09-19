#include "Arduino.h"
#include "Debug_LED.h"

Debug_LED::Debug_LED(int pin1, int pin2, int pin3)
{
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  _pin1 = pin1;
  _pin2 = pin2;
  _pin3 = pin3;
}

void Debug_LED::setColor(bool r, bool g, bool b)
{
  digitalWrite(_pin1, r ? HIGH : LOW);
  digitalWrite(_pin2, g ? HIGH : LOW);
  digitalWrite(_pin3, b ? HIGH : LOW);
}

void Debug_LED::off()
{
  this->setColor(false, false, false);
}
