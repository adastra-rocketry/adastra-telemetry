
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
  float g_x, g_y, g_z;
  sensors.readGyroscope(g_x, g_y, g_z);

  state.createDataPoint(pressure, temperature, acc_x, acc_y, acc_z, g_x, g_y, g_z);
  state.saveCurrentDataPoint();
  state.updateFlightState();
}



void setup() {
  led.setColor(true, false, false);
  
  if(DEBUG) {
    Serial.begin(9600);
    while (!Serial);  
    Serial.println("AdAstra Telemetry");
  }
  state.init();
  sensors.init(state);
  delay(1000);
  ble.Init();
  
  led.setColor(false, true, false);
}

void loop() {
  BLE.advertise();
  ble.DoLoop(state);
  
  long currentMillis = millis();
  if(currentMillis - previousMillis >= SAVE_INTERVAL) {
    readSensors();
    previousMillis = currentMillis;
  }
}
