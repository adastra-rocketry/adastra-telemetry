/*
  BluetoothStack.h - Library BT Communication
*/
#ifndef BluetoothStack_h
#define BluetoothStack_h

#include "Arduino.h"
#include "DataLogger.h"
#include <ArduinoBLE.h>
#include "Debug_LED.h"

class BluetoothStack
{
  public:
    BluetoothStack();
    void DoLoop(DataLogger logger);
    void Init();
  private:
    //DataLogger _logger;
    long _previousMillis = 0;
    Debug_LED _led{23,24,22}; 

    bool _shouldSendLog = false;
    
    BLEService _loggerService{"181C"};
    // BLE Battery Level Characteristic
    BLECharacteristic _loggerServiceChar{"2A3D",  // standard 16-bit characteristic UUID
        BLERead | BLENotify | BLEIndicate, sizeof(DataPoint)};
    BLEUnsignedCharCharacteristic _switchServiceChar{"19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite};

    BLECharacteristic _itemCountServiceChar{"2AC0", BLERead, sizeof(int)};

    void ProcessCommand(DataLogger logger);
    void WriteCount(DataLogger logger);
};

#endif
