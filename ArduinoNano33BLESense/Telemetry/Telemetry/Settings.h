#ifndef Settings_h
#define Settings_h


#define DEBUG false

#define SOUND true
#define BUZZER_PIN 8
#define EXTERNAL_BUZZER_PIN 9

#define SAVE_INTERVAL 400
#define STORAGE_SIZE 2048
#define RING_BUFFER_SIZE 5 // amount of preassure data to take into consideration to determine state transitions
#define DOWNLOAD_SPEED 80  //miliseconds between BLE updates

#endif
