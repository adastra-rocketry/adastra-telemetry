
import DataPointParser from "./datapointParser.js";

export default class BLEConnector {

  constructor(gui) {
      this.gui = gui;
      this.parser = new DataPointParser();

      this.commandToSend = null;
      this.temperatureData = [];
      this.pressureData = [];
      this.pressureDeltaData = [];
      this.kalmanPressureDeltaData = [];
      this.altitudeData = [];
      this.kalmanPressureData = [];
      this.kalmanAltitudeData = [];
      this.kalmanTemperatureData = [];
      this.accxData= [];
      this.accyData= [];
      this.acczData= [];
      this.gxData= [];
      this.gyData= [];
      this.gzData= [];
  
      this.itemCountCharacteristic = null;
      this.stateCharacteristic = null;
      this.commandCharacteristic = null;
      this.vehicleStateCharacteric = null;

      this.updateInterval;
      this.device = null;
      this.gui.commandListeners.push(this.processGUICommand.bind(this));
  }

  async processGUICommand(sender, command) {
    if(command == "connect") {
      await this.connectDevice();
    } else {
      this.commandToSend = command;
      
      if(await this.processCommand()) return;
    }
  }

  registerMachineUpdates() {
    this.vehicleStateCharacteristic.addEventListener('characteristicvaluechanged', this.vehicleStateCharacteristicChanged.bind(this));
    this.vehicleStateCharacteristic.startNotifications();
    this.itemCountCharacteristic.addEventListener('characteristicvaluechanged', this.itemCountCharacteristicChanged.bind(this));
    this.itemCountCharacteristic.startNotifications();
  }

  deregisterMachineUpdates() {
    this.vehicleStateCharacteristic.stopNotifications();
    this.vehicleStateCharacteristic.removeEventListener('characteristicvaluechanged', this.vehicleStateCharacteristicChanged.bind(this));
    this.itemCountCharacteristic.stopNotifications();
    this.itemCountCharacteristic.removeEventListener('characteristicvaluechanged', this.itemCountCharacteristicChanged.bind(this));

  }

  async sendMachineCommand(commandchar, arg1, arg2) {
    var buffer = new ArrayBuffer(12);
    var dataView = new DataView(buffer);

    arg1 = arg1 || 0.0;
    arg2 = arg2 || 0.0;
    let enc = new TextEncoder();
    let commandArray = enc.encode(commandchar);

    dataView.setUint8(0, commandArray[0]);
    dataView.setFloat32(4, arg1, true);
    dataView.setFloat32(8, arg2, true);

    let command = new Uint8Array(buffer);
    await this.commandCharacteristic.writeValue(command);
  }

  async vehicleStateCharacteristicChanged(event) {
    this.accxData = this.accxData.slice(-50);
    this.accyData = this.accyData.slice(-50);
    this.acczData = this.acczData.slice(-50);
    this.gxData = this.gxData.slice(-50);
    this.gyData = this.gyData.slice(-50);
    this.gzData = this.gzData.slice(-50);
    this.temperatureData = this.temperatureData.slice(-50);
    this.pressureData = this.pressureData.slice(-50);
    this.kalmanPressureData = this.kalmanPressureData.slice(-50);
    this.kalmanTemperatureData = this.kalmanTemperatureData.slice(-50);
    this.pressureDeltaData = this.pressureDeltaData.slice(-50);
    this.kalmanPressureDeltaData = this.kalmanPressureDeltaData.slice(-50);
    this.altitudeData = this.altitudeData.slice(-50);
    this.kalmanAltitudeData = this.kalmanAltitudeData.slice(-50);

    var value = event.target.value;
    var parsedValue = this.parser.parse(value);

    this.gui.setValue("temperature", parsedValue.kalmanTemperature);
    
    this.gui.setValue("pressure", parsedValue.kalmanPressure);
    this.gui.setValue("altitude", parsedValue.kalmanAltitude);
    this.gui.setValue("timestamp", parsedValue.timestamp);
    this.temperatureData.push(parsedValue.temperature);
    this.pressureData.push(parsedValue.pressure);
    this.pressureDeltaData.push(parsedValue.pressureDelta);
    this.kalmanPressureDeltaData.push(parsedValue.kalmanPressureDelta);
    this.kalmanAltitudeData.push(parsedValue.kalmanAltitude);
    this.kalmanPressureData.push(parsedValue.kalmanPressure);
    this.kalmanTemperatureData.push(parsedValue.kalmanTemperature);
    this.altitudeData.push(parsedValue.altitude);
    this.accxData.push(parsedValue.accX);
    this.accyData.push(parsedValue.accY);
    this.acczData.push(parsedValue.accZ);
    this.gxData.push(parsedValue.gX);
    this.gyData.push(parsedValue.gY);
    this.gzData.push(parsedValue.gZ);
    this.gui.setValue("temperatureGraph", [this.temperatureData, this.kalmanTemperatureData]);
    this.gui.setValue("pressureGraph", [this.pressureData, this.kalmanPressureData]);
    this.gui.setValue("altitudeGraph", [this.altitudeData, this.kalmanAltitudeData]);
    
    this.gui.setValue("acceleration", `X:${Math.round(parsedValue.accX*100000) / 100000} Y:${Math.round(parsedValue.accY*100000) / 100000} Z:${Math.round(parsedValue.accZ*100000) / 100000}`)
    this.gui.setValue("accxGraph", this.accxData);
    this.gui.setValue("accyGraph", this.accyData);
    this.gui.setValue("acczGraph", this.acczData);

    this.gui.setValue("gxGraph", this.gxData);
    this.gui.setValue("gyGraph", this.gyData);
    this.gui.setValue("gzGraph", this.gzData);
    this.gui.setValue("pressureDeltaGraph", [this.pressureDeltaData, this.kalmanPressureDeltaData]);

    this.gui.setValue("state", parsedValue.state);
  }

  async itemCountCharacteristicChanged() {
    let value = event.target.value;
    let count = value.getUint16(0);
    this.gui.setValue("itemCount", count);
  }

  async processCommand() {
    if(this.commandToSend != null) {
      if(this.commandToSend == "reset") {
        await this.sendMachineCommand('r');
      }
      if(this.commandToSend == "launchidle") {
        await this.sendMachineCommand('l');
      }
      if(this.commandToSend == "launchparameter") {
        await this.sendMachineCommand('s', this.gui.getValue("launchAltitude"), this.gui.getValue("launchPressure"));
      }
      if(this.commandToSend == "disconnect") {
        this.disconnectDevice();
      }
      this.commandToSend = null;
      return true;
    }
    return false;
  }

  async connectDevice() {
    try {
      console.log('Requesting Bluetooth Device...');
      this.device = await navigator.bluetooth.requestDevice({filters: [{name: 'AdAstra Telemetry'}], optionalServices: ['0000181c-0000-1000-8000-00805f9b34fb']});

      const server = await this.device.gatt.connect();
      const service = await server.getPrimaryService('0000181c-0000-1000-8000-00805f9b34fb');
      this.itemCountCharacteristic = await service.getCharacteristic(0x2ac0);
      this.stateCharacteristic = await service.getCharacteristic(0x2ac1);
      this.commandCharacteristic = await service.getCharacteristic('19b10001-e8f2-537e-4f6c-d104768a1214');
      this.vehicleStateCharacteristic = await service.getCharacteristic(0x2ac2);

      this.gui.elements.modal.hide();

      this.registerMachineUpdates();
    } catch(ex) {
        console.log(ex);
    }
  }

  async disconnectDevice() {
    console.log('Disconnecting from Bluetooth Device...');
    if (this.device.gatt.connected) {
      this.deregisterMachineUpdates();
      this.device.gatt.disconnect();
      this.gui.elements.modal.show();
    } else {
      console.log('> Bluetooth Device is already disconnected');
    }
  }
}