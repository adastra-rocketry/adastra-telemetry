
import DataPointParser from "./datapointParser.js";
import Download from "./download.js";

export default class BLEConnector {

  constructor(gui) {
      this.gui = gui;
      this.parser = new DataPointParser();
      this.downloader = new Download();

      this.commandToSend = null;
      this.temperatureData = [];
      this.pressureData = [];
      this.altitudeData = [];
      this.accxData= [];
      this.accyData= [];
      this.acczData= [];
  
      this.itemCountCharacteristic = null;
      this.stateCharacteristic = null;
      this.commandCharacteristic = null;
      this.vehicleStateCharacteric = null;
      this.downloadCharacteristic = null;

      this.updateInterval;
      this.gui.commandListeners.push(this.processGUICommand.bind(this));
  }

  async processGUICommand(sender, command) {
    if(command == "connect") {
      await this.connectDevice();
    } else {
      this.commandToSend = command;
    }
  }

  registerMachineUpdates() {
    this.updateInterval = setInterval(this.readValues.bind(this), 400);
  }

  deregisterMachineUpdates() {
    clearInterval(this.updateInterval)
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

  async readValues() {
    if(await this.processCommand()) return;

    this.accxData = this.accxData.slice(-50);
    this.accyData = this.accyData.slice(-50);
    this.acczData = this.acczData.slice(-50);
    this.temperatureData = this.temperatureData.slice(-50);
    this.pressureData = this.pressureData.slice(-50);
    this.altitudeData = this.altitudeData.slice(-50);

    let value = await this.itemCountCharacteristic.readValue();
    let count = value.getUint16(0);
    this.gui.setValue("itemCount", count);
    value = await this.stateCharacteristic.readValue();
    let state = value.getUint16(0);
    this.gui.setValue("state", state);

    value = await this.vehicleStateCharacteristic.readValue();
    var parsedValue = this.parser.parse(value);

    this.gui.setValue("temperature", parsedValue.temperature);
    
    this.gui.setValue("pressure", parsedValue.pressure);
    this.gui.setValue("altitude", parsedValue.altitude);
    this.gui.setValue("timestamp", parsedValue.timestamp);
    this.temperatureData.push(parsedValue.temperature);
    this.pressureData.push(parsedValue.pressure);
    this.altitudeData.push(parsedValue.altitude);
    this.accxData.push(parsedValue.accX);
    this.accyData.push(parsedValue.accY);
    this.acczData.push(parsedValue.accZ);
    this.gui.setValue("temperatureGraph", this.temperatureData);
    this.gui.setValue("pressureGraph", this.pressureData);
    this.gui.setValue("altitudeGraph", this.altitudeData);
    
    this.gui.setValue("acceleration", `X:${Math.round(parsedValue.accX*100000) / 100000} Y:${Math.round(parsedValue.accY*100000) / 100000} Z:${Math.round(parsedValue.accZ*100000) / 100000}`)
    this.gui.setValue("accxGraph", this.accxData);
    this.gui.setValue("accyGraph", this.accyData);
    this.gui.setValue("acczGraph", this.acczData);
  }

  async processCommand() {
    if(this.commandToSend != null) {
      if(this.commandToSend == "reset") {
        if (confirm('Are you sure you want to reset the device?')) {
          await this.sendMachineCommand('r');
        }
      }
      if(this.commandToSend == "download") {
        this.downloader.reset();
        this.deregisterMachineUpdates();
        await this.downloadCharacteristic.startNotifications();
        await this.sendMachineCommand('d');
      }
      if(this.commandToSend == "launchidle") {
        await this.sendMachineCommand('l');
      }
      if(this.commandToSend == "launchparameter") {
        await this.sendMachineCommand('s', this.gui.getValue("launchAltitude"), this.gui.getValue("launchPressure"));
      }
      this.commandToSend = null;
      return true;
    }
    return false;
  }

  handleDownloadNotifications(event) {
    let value = event.target.value;
    let parsedValue = this.parser.parse(value, true);

    this.downloader.addNewLine(parsedValue);
    console.log(parsedValue);

    if(parsedValue.type == 1) {
      this.downloader.download();
      this.registerMachineUpdates();
    }
  }

  async connectDevice() {
    try {
      console.log('Requesting Bluetooth Device...');
      const device = await navigator.bluetooth.requestDevice({filters: [{name: 'AdAstra Telemetry'}], optionalServices: ['0000181c-0000-1000-8000-00805f9b34fb']});

      const server = await device.gatt.connect();
      const service = await server.getPrimaryService('0000181c-0000-1000-8000-00805f9b34fb');
      this.itemCountCharacteristic = await service.getCharacteristic(0x2ac0);
      this.stateCharacteristic = await service.getCharacteristic(0x2ac1);
      this.commandCharacteristic = await service.getCharacteristic('19b10001-e8f2-537e-4f6c-d104768a1214');
      this.vehicleStateCharacteristic = await service.getCharacteristic(0x2ac2);
      this.downloadCharacteristic = await service.getCharacteristic(0x2a3d);
      this.downloadCharacteristic.addEventListener('characteristicvaluechanged', this.handleDownloadNotifications.bind(this));

      this.gui.elements.modal.remove();

      this.registerMachineUpdates();
    } catch(ex) {
        console.log(ex);
    }
  }
}