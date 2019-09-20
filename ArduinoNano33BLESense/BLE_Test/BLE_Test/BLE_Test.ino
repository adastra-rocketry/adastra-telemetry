#include <Arduino_LPS22HB.h>
#include "DataLogger.h"
#include "BluetoothStack.h"
#include "Debug_LED.h"

DataLogger logger;
BluetoothStack ble;
unsigned long previousMillis = 0;

Debug_LED led(23,24,22); 

void createPressureDatapoint() {
  float pressure = BARO.readPressure();
  DataPoint newItem = {Pressure, millis(), pressure};
  logger.saveValue(newItem);
}

void createTemperatureDatapoint() {
  float pressure = BARO.readPressure();
  DataPoint newItem = {Temperature, millis(), pressure};
  logger.saveValue(newItem);
}


void setup() {
  led.setColor(true, false, false);
  // put your setup code here, to run once:
  Serial.begin(9600);
  //while (!Serial);

  Serial.println("AdAstra Telemetry");
  led.setColor(false, true, false);
  
  if (!BARO.begin()) {
    Serial.println("Failed to initialize pressure sensor!");
    while (1);
  }
  ble.Init();
}

void loop() {
  // put your main code here, to run repeatedly:

  long currentMillis = millis();
  // if 200ms have passed, check the battery level:
  if (logger.hasSpaceLeft() && (currentMillis - previousMillis >= 200)) {
    previousMillis = currentMillis;
    Serial.println("Saving new Value");
    createPressureDatapoint();
  }
  
  ble.DoLoop(logger);
}
