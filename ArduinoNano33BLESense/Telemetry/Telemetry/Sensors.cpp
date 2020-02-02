#include "Arduino.h"
#include "Sensors.h"
#include "Settings.h"
#include <Arduino_LPS22HB.h>
#include <Arduino_HTS221.h>
#include <Arduino_LSM9DS1.h>

Sensors::Sensors() {
  
}

void Sensors::init(State& initState) {
  state = initState;
  initIMU();
  initTemperatureSensor();
  initBarometer();
  state.sound.stopSound();
}

void Sensors::readAcceleration(float &acc_x, float &acc_y, float &acc_z ) {
  acc_x = -999, acc_y = -999, acc_z = -999;
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(acc_x,acc_y,acc_z);
  }
}

void Sensors::readGyroscope(float &g_x, float &g_y, float &g_z) {
  g_x = -999, g_y = -999, g_z = -999;
  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(g_x,g_y,g_z);
  }
}

float Sensors::readPressure() {
  return BARO.readPressure();
}

float Sensors::readTemperature() {
  return HTS.readTemperature();
}

void Sensors::initBarometer() {
  state.sound.playSound(262, 1000);
  delay(500);
  if (!BARO.begin()) {
    if(DEBUG) Serial.println("Failed to initialize pressure sensor!");
    BARO.end();
    initBarometer();
  }
}

void Sensors::initIMU() {
  state.sound.playSound(292, 1000);
  delay(500);
  if (!IMU.begin()) {
    if(DEBUG) Serial.println("Failed to initialize IMU!");
    IMU.end();
    initIMU();
  }
}

void Sensors::initTemperatureSensor() {
  state.sound.playSound(196, 1000);
  delay(1000);
  if (!HTS.begin()) {
    if(DEBUG) Serial.println("Failed to init temperature sensor!");
    HTS.end();
    initTemperatureSensor();
  } else {
    float temperature = HTS.readTemperature();
    if(DEBUG) {
      Serial.print("Initial Temperature: ");
      Serial.println(temperature);
    }
    
    if(temperature == 0.00f) {
      HTS.end();
      if(DEBUG) Serial.println("Temperature sensor reads strange values!");
      initTemperatureSensor();
    }
  }
}
