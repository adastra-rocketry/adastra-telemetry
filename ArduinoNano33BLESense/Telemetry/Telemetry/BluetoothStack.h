/*
  BluetoothStack.h - Library BT Communication
*/
#ifndef BluetoothStack_h
#define BluetoothStack_h

#include "Arduino.h"
#include "DataLogger.h"
#include <ArduinoBLE.h>
#include "Debug_LED.h"
#include "State.h"

struct Command {
  char Type;
  float Arg1;
  float Arg2;
};

class BluetoothStack
{
  public:
    BluetoothStack();
    void DoLoop(State& state);
    void Init();
  private:
    Debug_LED _led{23,24,22};

    unsigned long previousMillis = 0;
    
    BLEService _loggerService{"181C"};
    BLECharacteristic _commandServiceChar{"19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite, sizeof(Command)};

    BLECharacteristic _itemCountServiceChar{"2AC0", BLERead | BLEIndicate, sizeof(int)};
    BLECharacteristic _stateServiceChar{"2AC1", BLERead | BLEIndicate, sizeof(int)};
    BLECharacteristic _currentDataPointServiceChar{"2AC2", BLERead | BLENotify | BLEIndicate, sizeof(DataPoint)};

    void ProcessCommand(State& state);
    void WriteCount(State& state);
    void WriteState(State& state);
    void WriteCurrentDataPoint(State& state);
};

#endif
