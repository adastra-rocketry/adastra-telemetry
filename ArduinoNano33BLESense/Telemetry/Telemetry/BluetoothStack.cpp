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
  _loggerService.addCharacteristic(_commandServiceChar);
  _loggerService.addCharacteristic(_itemCountServiceChar);
  _loggerService.addCharacteristic(_stateServiceChar);
  _loggerService.addCharacteristic(_currentDataPointServiceChar);
  
  BLE.addService(_loggerService);
  _commandServiceChar.setValue(0);

  
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
    WriteCount(state);
    WriteState(state);
    WriteCurrentDataPoint(state);

    if (central.connected()) {
      ProcessCommand(state);
      _led.setColor(false, false, true);
    }
    _led.setColor(false, true, false);
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
  if (_commandServiceChar.written()) {
    Command command;
    const uint8_t* b;
    b = _commandServiceChar.value();
    memcpy(&command, b, sizeof(command));
    
    if(DEBUG) {
      Serial.print("Got new command:");
      Serial.print(command.Type);
      Serial.println();
      Serial.print(command.Arg1, 3);
      Serial.println();
      Serial.print(command.Arg2, 3);
      Serial.println();
    }

    switch(command.Type) {
      case 'd':
        state.vehicleState = Vehicle_State::Landed;
        WriteState(state);
        break;
      case 'r':
        state.logger.empty();
        WriteCount(state);
        break;
      case 'l':
        state.vehicleState = Vehicle_State::LaunchIdle;
        WriteState(state);
        break;
      case 's':
        state.LaunchAltitude = command.Arg1;
        state.PressureNN = command.Arg2;
        break;
      default:
        if(DEBUG) {
          Serial.print("Unknown command: ");
          Serial.println(command.Type);
        }
    }
    
  }
}
