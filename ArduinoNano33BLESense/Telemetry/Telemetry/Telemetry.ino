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
  DataPoint newItem = {millis(), pressure, temperature, acc_x, acc_y, acc_z};
  logger.saveValue(newItem);
}


void setup() {
  led.setColor(true, false, false);
  // put your setup code here, to run once:
  Serial.begin(9600);
  //while (!Serial);

  Serial.println("AdAstra Telemetry");
  
  
  if (!BARO.begin()) {
    Serial.println("Failed to initialize pressure sensor!");
    while (1);
  }

  if (!HTS.begin()) {
    Serial.println("Failed to initialize humidity temperature sensor!");
    while (1);
  }

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  
  ble.Init();
  led.setColor(false, true, false);
}

void loop() {
  // put your main code here, to run repeatedly:

  long currentMillis = millis();
  // if 200ms have passed
  if (logger.hasSpaceLeft() && (currentMillis - previousMillis >= 400)) {
    previousMillis = currentMillis;
    Serial.println("Saving new Value");
    createDatapoint();
    BLE.advertise();
  }
  
  ble.DoLoop(logger);
}
