#include "Arduino.h"
#include <ArduinoBLE.h>
#include "BluetoothStack.h"
#include "DataLogger.h"
#include "Debug_LED.h"
#include "Settings.h"
#include "State.h"

BluetoothStack::BluetoothStack() {
  
}

void BluetoothStack::Init() {
  // begin initialization
  if (!BLE.begin()) {
    if(DEBUG) Serial.println("starting BLE failed!");
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
  _loggerService.addCharacteristic(_stateServiceChar);
  _loggerService.addCharacteristic(_currentDataPointServiceChar);
  
  BLE.addService(_loggerService); // Add the battery service
  _switchServiceChar.setValue(0);

  
  /* Start advertising BLE.  It will start continuously transmitting BLE
     advertising packets and will be visible to remote BLE central devices
     until it receives a new connection */

  // start advertising
  BLE.advertise();
  if(DEBUG) Serial.println("Bluetooth device active, waiting for connections...");
}

void BluetoothStack::DoLoop(State& state) {
  BLEDevice central = BLE.central();
  if (central) {
    if(DEBUG) Serial.print("Connected to central: ");
    // print the central's BT address:
    if(DEBUG) Serial.println(central.address());

    WriteCount(state);
    WriteState(state);
    WriteCurrentDataPoint(state);

    if (central.connected()) {
      ProcessCommand(state);
      _led.setColor(false, false, true);
      bool hasNextEntry = state.logger.hasNextEntry();
      if(_shouldSendLog && hasNextEntry) {
        _transferInProgress = true;
        long currentMillis = millis();
          
        if (currentMillis - _previousMillis >= DOWNLOAD_SPEED) {
          DataPoint point = state.logger.getNextEntry();
          _transferObject.Data = point;
          _transferObject.Type = Transfer_Type::Data; 
          unsigned char b[sizeof(_transferObject)];
          memcpy(b, &_transferObject, sizeof(_transferObject));
          _loggerServiceChar.writeValue(b, sizeof(b));
          _previousMillis = currentMillis;
        }
      } else {
        if(_transferInProgress) {
          _transferObject.Type = Transfer_Type::End; 
          unsigned char b[sizeof(_transferObject)];
          memcpy(b, &_transferObject, sizeof(_transferObject));

          _loggerServiceChar.writeValue(b, sizeof(b));
          _transferInProgress = false;
        }

        _shouldSendLog = false;
      }
    }
    _led.setColor(false, true, false);
    
    if(DEBUG) {
      Serial.print("Disconnected from central: ");
      Serial.println(central.address());   
    }
  }
}

void BluetoothStack::WriteCount(State& state) {
    int count = state.logger.getCounter();
    unsigned char bytes[2];
    bytes[0] = (count >> 8) & 0xFF;
    bytes[1] = count & 0xFF;
    
    _itemCountServiceChar.writeValue(bytes, sizeof(bytes)); // and publish it via BT
}

void BluetoothStack::WriteState(State& stateObj) {
    int state = stateObj.vehicleState;
    unsigned char bytes[2];
    bytes[0] = (state >> 8) & 0xFF;
    bytes[1] = state & 0xFF;
    
    _stateServiceChar.writeValue(bytes, sizeof(bytes)); // and publish it via BT
}


void BluetoothStack::WriteCurrentDataPoint(State& stateObj) {
    unsigned char b[sizeof(stateObj.currentDataPoint)];
    memcpy(b, &stateObj.currentDataPoint, sizeof(stateObj.currentDataPoint));
    _currentDataPointServiceChar.writeValue(b, sizeof(b)); // and publish it via BT
}

void BluetoothStack::ProcessCommand(State& state) {
  if (_switchServiceChar.written()) {
    char command = _switchServiceChar.value();
    if(DEBUG) {
      Serial.print("Got new command:");
      Serial.println(command);
    }

    switch(command) {
      case 'd':
        state.vehicleState = Vehicle_State::Landed;
        WriteState(state);
        _shouldSendLog = true;
        break;
      case 'r':
        state.logger.empty();
        WriteCount(state);
        break;
      case 'l':
        state.vehicleState = Vehicle_State::LaunchIdle;
        WriteState(state);
        break;
      default:
        if(DEBUG) {
          Serial.print("Unknown command: ");
          Serial.println(command);
        }
    }
    
  }
}
