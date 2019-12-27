#include "Settings.h"
#include "DataLogger.h"
#include "BluetoothStack.h"
#include "Debug_LED.h"
#include "Sensors.h"
#include "State.h"

BluetoothStack ble;
unsigned long previousMillis = 0;

State state;

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

  state.createDataPoint(pressure, temperature, acc_x, acc_y, acc_z);
}



void setup() {
  led.setColor(true, false, false);
  
  if(DEBUG) {
    Serial.begin(9600);
    while (!Serial);  
    Serial.println("AdAstra Telemetry");
  }
  state.init();
  sensors.init();
  delay(1000);
  ble.Init();
  
  led.setColor(false, true, false);
}

void loop() {
  BLE.advertise();
  ble.DoLoop(state);
  
  long currentMillis = millis();
  // if 200ms have passed
  if(currentMillis - previousMillis >= SAVE_INTERVAL) {
    readSensors();
    previousMillis = currentMillis;
  }
}
