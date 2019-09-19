#include <Arduino_LPS22HB.h>
#include "Debug_LED.h"

Debug_LED led(23,24,22); 

void setup() {
  led.setColor(true,false,false);
  delay(500);
  // put your setup code here, to run once:
  Serial.begin(9600);
  if (!BARO.begin()) {
    Serial.println("Failed to initialize pressure sensor!");
    while (1);
  }
  led.setColor(false,true,false);
}

void loop() {
  // put your main code here, to run repeatedly:
  float pressure = BARO.readPressure();

  //Serial.print("Pressure = ");
  Serial.println(pressure);
  //Serial.println(" kPa");
  delay(1000);
  led.setColor(false,false,true);
  delay(50);
  led.setColor(false,true,false);
}
