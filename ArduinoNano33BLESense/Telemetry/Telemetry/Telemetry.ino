#include "Settings.h"

#include <Arduino_LPS22HB.h>
#include <Arduino_HTS221.h>
#include <Arduino_LSM9DS1.h>
#include "DataLogger.h"
#include "BluetoothStack.h"
#include "Debug_LED.h"

DataLogger logger;
BluetoothStack ble;
unsigned long previousMillis = 0;

Debug_LED led(23,24,22); 

void createDatapoint() {
  float acc_x = -999, acc_y = -999, acc_z = -999;
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(acc_x,acc_y,acc_z);
  }
  float temperature = HTS.readTemperature();
  float pressure = BARO.readPressure();

  if(DEBUG) {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println("°C");
  }
  DataPoint newItem = {millis(), pressure, temperature, acc_x, acc_y, acc_z};
  logger.saveValue(newItem);
}

void initTemperatureSensor() {
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

void setup() {
  led.setColor(true, false, false);
  // put your setup code here, to run once:
  
  if(DEBUG) {
    Serial.begin(9600);
    while (!Serial);
  
    Serial.println("AdAstra Telemetry");
  }
  
  if (!BARO.begin()) {
    if(DEBUG) Serial.println("Failed to initialize pressure sensor!");
    while (1);
  }

  initTemperatureSensor();

  if (!IMU.begin()) {
    if(DEBUG) Serial.println("Failed to initialize IMU!");
    while (1);
  }

  delay(3000);
  ble.Init();
  led.setColor(false, true, false);
}

void loop() {
  // put your main code here, to run repeatedly:

  long currentMillis = millis();
  // if 200ms have passed
  if (logger.hasSpaceLeft() && (currentMillis - previousMillis >= SAVE_INTERVAL)) {
    previousMillis = currentMillis;
    if(DEBUG) Serial.println("Saving new Value");
    createDatapoint();
    BLE.advertise();
  }
  
  ble.DoLoop(logger);
}
