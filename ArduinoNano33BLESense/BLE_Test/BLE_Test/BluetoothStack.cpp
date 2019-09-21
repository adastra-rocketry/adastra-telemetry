#include "Arduino.h"
#include <ArduinoBLE.h>
#include "BluetoothStack.h"
#include "DataLogger.h"
#include "Debug_LED.h"

BluetoothStack::BluetoothStack() {
  
}

void BluetoothStack::Init() {
  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    _led.setColor(true, false, false);
    while (1);
  }  
  /* Set a local name for the BLE device
     This name will appear in advertising packets
     and can be used by remote devices to identify this BLE device
     The name can be changed but maybe be truncated based on space left in advertisement packet
  */
  BLE.setLocalName("AdAstra Telemetry");
  BLE.setAdvertisedService(_loggerService); // add the service UUID
  _loggerService.addCharacteristic(_loggerServiceChar);
  _loggerService.addCharacteristic(_switchServiceChar);
  _loggerService.addCharacteristic(_itemCountServiceChar);
  BLE.addService(_loggerService); // Add the battery service
  //_loggerServiceChar.writeValue(0); // set initial value for this characteristic
  _switchServiceChar.setValue(0);

  
  /* Start advertising BLE.  It will start continuously transmitting BLE
     advertising packets and will be visible to remote BLE central devices
     until it receives a new connection */

  // start advertising
  BLE.advertise();
  Serial.println("Bluetooth device active, waiting for connections...");
}

void BluetoothStack::DoLoop(DataLogger logger) {
  BLEDevice central = BLE.central();
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's BT address:
    Serial.println(central.address());

    WriteCount(logger);

    while (central.connected()) {
      ProcessCommand(logger);
      _led.setColor(false, false, true);
      bool hasNextEntry = logger.hasNextEntry();
      if(_shouldSendLog && hasNextEntry) {
        long currentMillis = millis();
        // if 200ms have passed, check the battery level:
          
        if (currentMillis - _previousMillis >= 200) {
          DataPoint point = logger.getNextEntry();
          unsigned char b[sizeof(point)];
          memcpy(b, &point, sizeof(point));
          _loggerServiceChar.writeValue(b, sizeof(b));
          Serial.print("new value: ");
          //Serial.println(values);
          _previousMillis = currentMillis;
        }
      } else {
        _shouldSendLog = false;
      }
    }
    _led.setColor(false, true, false);
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());    
  }
}

void BluetoothStack::WriteCount(DataLogger logger) {
    Serial.print("Current count of gathered data: ");
    int count = logger.getCounter();
    Serial.println(count);
    unsigned char bytes[2];
    bytes[0] = (count >> 8) & 0xFF;
    bytes[1] = count & 0xFF;
    
    _itemCountServiceChar.writeValue(bytes, sizeof(bytes)); // and publish it via BT
    Serial.println("Wrote count char");
}

void BluetoothStack::ProcessCommand(DataLogger logger) {
  if (_switchServiceChar.written()) {
    char command = _switchServiceChar.value();
    switch(command) {
      case 'd':
        _shouldSendLog = true;
        break;
      case 'c':
        logger.empty();
        //WriteCount(logger);
        break;
      default:
        Serial.print("Unknown command: ");
        Serial.println(command);
    }
    
  }
}
