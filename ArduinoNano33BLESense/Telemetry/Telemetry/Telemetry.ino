#include "Settings.h"
#include "DataLogger.h"
#include "BluetoothStack.h"
#include "Debug_LED.h"
#include "Sensors.h"

DataLogger logger;
BluetoothStack ble;
unsigned long previousMillis = 0;

Vehicle_State State = Vehicle_State::LaunchIdle;

Debug_LED led(23,24,22);

Sensors sensors;

void readSensors() {
  float temperature = sensors.readTemperature();
  float pressure = sensors.readPressure();
  float acc_x, acc_y, acc_z;
  sensors.readAcceleration(acc_x, acc_y, acc_z);

  if(DEBUG) {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println("°C");
  }

  createDataPoint(pressure, temperature, acc_x, acc_y, acc_z);
}

void createDataPoint(float pressure, float temperature, float acc_x, float acc_y, float acc_z) {
  DataPoint newItem = {State, millis(), pressure, temperature, acc_x, acc_y, acc_z};
  logger.saveValue(newItem);
}

void setup() {
  led.setColor(true, false, false);
  
  if(DEBUG) {
    Serial.begin(9600);
    while (!Serial);  
    Serial.println("AdAstra Telemetry");
  }
  sensors.init();
  delay(1000);
  ble.Init();
  
  led.setColor(false, true, false);
}

void loop() {
  // put your main code here, to run repeatedly:

  long currentMillis = millis();
  // if 200ms have passed
  if (State > 0 && logger.hasSpaceLeft() && (currentMillis - previousMillis >= SAVE_INTERVAL)) {
    previousMillis = currentMillis;
    readSensors();
  }
  
  BLE.advertise();
  ble.DoLoop(logger, State);
}
