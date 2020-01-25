#ifndef Sound_h
#define Sound_h

#include "Arduino.h"

class Sound
{
  public:
    Sound();
    void playSound(float freq, int t);
    void stopSound();
    void playError(unsigned char code);
};

#endif
