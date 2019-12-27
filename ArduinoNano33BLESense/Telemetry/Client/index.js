const noble = require('noble-uwp');
const readline = require('readline');
const createCsvWriter = require('csv-writer').createObjectCsvWriter;
const calcAlt = require('./calc_altitude.js');


const deviceUUID = "f1e2c75cb032";

if (noble.state === "poweredOn") {
  noble.startScanning([], true);
}

noble.on("stateChange", function(state) {
  console.log("BLE State change: " + state);
  if (state === "poweredOn") {
    noble.startScanning([], true);
  } else {
    noble.stopScanning();
  }
});

let found = false;
noble.on('discover', function(peripheral) {
  if (peripheral.id == deviceUUID && !found) {
    found = true;
    console.log('peripheral with ID ' + peripheral.id + ' found');
    noble.stopScanning();
    enterInteractiveMode(peripheral, () => {
      console.log("Starting scan...");
      found = false;
      noble.startScanning();
    });
  }
});

function enterInteractiveMode(peripheral, callback) {
  let rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
  });
  peripheral.connect(function(error) {
    peripheral.discoverAllServicesAndCharacteristics(function(error, services, characteristics) {
      console.log("Connected to " + peripheral.advertisement.localName);
      console.log("Possible commands are:");
      console.log("exit,       e               exits the programm");
      console.log("state,      s               state of the device");
      console.log("vehicle,    v               last measurements of vehicle");
      console.log("launch,     l               set state to launch idle");
      console.log("count,      c               current count of datapoints");
      console.log("reset,      r               reset datalogger");
      console.log("download,   d               download data");
      repl(rl, characteristics, peripheral, () => {
        peripheral.disconnect();
        callback();
      });
    });
  });
}

function repl(rl, characteristics, peripheral, callback) {
      let count = characteristics.find(f => f.uuid == "2ac0");
      let state = characteristics.find(f => f.uuid == "2ac1");
      let vehiclestate = characteristics.find(f => f.uuid == "2ac2");
      let logger = characteristics.find(f => f.uuid == "2a3d");
      let command = characteristics.find(f => f.uuid == "19b10001e8f2537e4f6cd104768a1214");

      let cont = () => {
        repl(rl, characteristics, peripheral, callback); //Calling this function again to ask new question
      };

      let writeCommand = (c, cb) => {
        let buffer = Buffer.from(c, 'ascii');
        command.write(buffer, false, cb);
      };

      let readCurrentVehicleState = cb => {
        let states = [
          "Idle",
          "LaunchIdle",
          "Ascending",
          "Descending",
          "Landed"
        ];
        state.read((error, data) => {
          data = data.readInt16BE();
          if(error) {
            console.log("Error while retrieving state");
          } else {
            console.log(`System is currently in state ${states[data]}`);
          }
          cb();
        });
      };

      rl.question('Command: ', function (answer) {
        switch (answer) {
          case 'exit':
          case 'e':
            rl.close();
            peripheral.disconnect();
            process.exit();
            break;
          case 'reset':
          case 'r':
            writeCommand('r', cont);
            break;
          case 'launch':
          case 'l':
            writeCommand('l', readCurrentVehicleState(cont));
            break;
          case 'count':
          case 'c':
            count.read((error, data) => {
              console.log(data.length)
              data = data.readInt16BE();
              if(error) {
                console.log("Error while retrieving count");
              } else {
                console.log(`There are currently ${data} datapoints in the logger.`);
              }
              cont();
            });
            break;
          case 'v':
            let processVehicleData = (data) => {
              let commandObject = {};
              //console.log(data.length)
              commandObject['state'] = data.readInt16LE(0);
              commandObject['timestamp'] = data.readUInt32LE(4);
              commandObject['pressure'] = Math.round(data.readFloatLE(8) * 10000) / 10000;
              commandObject['temperature'] = Math.round(data.readFloatLE(12) * 10000) / 10000;
              commandObject['acc_x'] = Math.round(data.readFloatLE(16) * 10000) / 10000;
              commandObject['acc_y'] = Math.round(data.readFloatLE(20) * 10000) / 10000;
              commandObject['acc_z'] = Math.round(data.readFloatLE(24) * 10000) / 10000;
              commandObject['calcedAlt'] = calcAlt(commandObject['pressure'] * 10, commandObject['temperature']);
              
              commandObject['pressureDelta'] = Math.round(data.readFloatLE(28) * 10000) / 10000;
              commandObject['pressureDeltaMid'] = Math.round(data.readFloatLE(32) * 10000) / 10000;
              return commandObject;
            };
            vehiclestate.read((error, data) => {
              var datapoint = processVehicleData(data);
              console.log(`
                State: ${datapoint.state}
                Timestamp: ${datapoint.timestamp}
                
                PressureDelta: ${datapoint.pressureDelta} PressureDeltaMid: ${datapoint.pressureDeltaMid}

                Pressure: ${datapoint.pressure}
                Temperature: ${datapoint.temperature}
                
                Altitute (approx): ${datapoint.calcedAlt}
              `);
              cont();
            });
            break;
          case 'state':
          case 's':
            readCurrentVehicleState(cont);
            break;
          case 'download':
          case 'd':
            let fileName = 'out.csv';
            const csvWriter = createCsvWriter({
              path: fileName,
              fieldDelimiter : ';',
              header: [
                {id: 'state', title: 'Vehicle state'},
                {id: 'timestamp', title: 'Timestamp'},
                {id: 'pressure', title: 'Pressure'},
                {id: 'pressureDelta', title: 'Pressure delta'},
                {id: 'pressureDeltaMid', title: 'Pressure delta mid'},
                {id: 'temperature', title: 'Temperature'},
                {id: 'acc_x', title: 'Acceleration X'},
                {id: 'acc_y', title: 'Acceleration Y'},
                {id: 'acc_z', title: 'Acceleration Z'},
                {id: 'calcedAlt', title: 'Altitude (approx)'}
              ]
            });
            let processData = (data) => {
              let commandObject = {};
              //console.log(data.length)
              commandObject['type'] = data.readInt16LE();
              commandObject['state'] = data.readInt16LE(4);
              commandObject['timestamp'] = data.readUInt32LE(8);
              commandObject['pressure'] = Math.round(data.readFloatLE(12) * 10000) / 10000;
              commandObject['temperature'] = Math.round(data.readFloatLE(16) * 10000) / 10000;
              commandObject['acc_x'] = Math.round(data.readFloatLE(20) * 10000) / 10000;
              commandObject['acc_y'] = Math.round(data.readFloatLE(24) * 10000) / 10000;
              commandObject['acc_z'] = Math.round(data.readFloatLE(28) * 10000) / 10000;
              commandObject['calcedAlt'] = calcAlt(commandObject['pressure'] * 10, commandObject['temperature']);
              
              commandObject['pressureDelta'] = Math.round(data.readFloatLE(32) * 10000) / 10000;
              commandObject['pressureDeltaMid'] = Math.round(data.readFloatLE(36) * 10000) / 10000;
              return commandObject;
            };


            logger.on('data', (data, isNotification) => {
              if(isNotification) {
                //console.log(data);
                let obj = processData(data);
                if(obj.type == 0) {
                  csvWriter.writeRecords([obj]);
                } else {
                  console.log("Transfer done");
                  cont();
                }
              }
            });

            logger.subscribe((error) => {
              //console.log("notify func" + error);
              
              writeCommand('d', () => {
                //cont();
              });
            });
            
            break;
          default:
            console.log(`Unknown command: ${answer}`)
            cont();
            break;
        }
  });
}