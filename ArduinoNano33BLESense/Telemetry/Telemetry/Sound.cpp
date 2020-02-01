#include "Sound.h"
#include "Settings.h"

Sound::Sound() {
  
}

void Sound::playSound(float freq, int t) {
  if(SOUND) {
    tone(BUZZER_PIN, freq, t);
  }
  if(EXTERNAL_SOUND) {
    tone(EXTERNAL_BUZZER_PIN, freq, t);
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

void Sound::playError(unsigned char code){
  delay(500);
  if(SOUND) {
    int what_bit_i_am_testing = 0;
    
    while (what_bit_i_am_testing < 8) {
      if (code & 0x01) {
         playSound(300, 100);
      }
      else {
         playSound(400, 100);
         
      }
      
      delay(180);
    
      what_bit_i_am_testing++;
      code = code >> 1;
    }
    delay(1000);
  }
}
