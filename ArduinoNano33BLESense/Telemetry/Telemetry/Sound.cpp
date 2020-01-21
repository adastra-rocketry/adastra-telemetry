#include "Sound.h"
#include "Settings.h"

Sound::Sound() {
  
}

void Sound::playSound(float freq, int t) {
  if(SOUND) {
    tone(BUZZER_PIN, freq, t * 1000);
    tone(EXTERNAL_BUZZER_PIN, freq, t * 1000);
  }
}

void Sound::stopSound() {
  if(SOUND) {
    noTone(BUZZER_PIN);
    noTone(EXTERNAL_BUZZER_PIN);
  }
}
