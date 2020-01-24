#include "Sound.h"
#include "Settings.h"

Sound::Sound() {
  
}

void Sound::playSound(float freq, int t) {
  if(SOUND) {
    tone(BUZZER_PIN, freq, t * 1000);
  }
  if(EXTERNAL_SOUND) {
    tone(EXTERNAL_BUZZER_PIN, freq, t * 1000);
  }
}

void Sound::stopSound() {
  if(SOUND) {
    noTone(BUZZER_PIN);
  }
  if(EXTERNAL_SOUND) {
    noTone(EXTERNAL_BUZZER_PIN);
  }
}
