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

noble.on('discover', function(peripheral) {
  if (peripheral.id == deviceUUID) {
    console.log('peripheral with ID ' + peripheral.id + ' found');
    noble.stopScanning();
    enterInteractiveMode(peripheral, () => {
      console.log("Starting scan...");
      noble.startScanning();
    });
  }
});

function enterInteractiveMode(peripheral, callback) {
  let rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
  });
  console.log("Connected to " + peripheral.advertisement.localName);
  console.log("Possible commands are:");
  console.log("exit,       e               exits the programm");
  console.log("search,     s               search for other loggers");
  console.log("count,      c               current count of datapoints");
  console.log("reset,      r               reset datalogger");
  console.log("download,   d               download data");
  peripheral.connect(function(error) {
    peripheral.discoverAllServicesAndCharacteristics(function(error, services, characteristics) {
      repl(rl, characteristics, peripheral, () => {
        peripheral.disconnect();
        callback();
      });
    });
  });
}

function repl(rl, characteristics, peripheral, callback) {

      //console.log(characteristics);
      let count = characteristics.find(f => f.uuid == "2ac0");
      let logger = characteristics.find(f => f.uuid == "2a3d");
      let command = characteristics.find(f => f.uuid == "19b10001e8f2537e4f6cd104768a1214");



      let cont = () => {
        repl(rl, characteristics, peripheral, callback); //Calling this function again to ask new question
      };

      let writeCommand = (c, cb) => {
        let buffer = Buffer.from(c, 'ascii');
        command.write(buffer, false, cb);
      };

      rl.question('Command: ', function (answer) {
        switch (answer) {
          case 'search':
          case 's':
            rl.close();
            callback();
            break;
          case 'exit':
          case 'e':
            rl.close();
            peripheral.disconnect();
            process.exit();
            break;
          case 'reset':
          case 'r':
            writeCommand('c', cont);
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
          case 'download':
          case 'd':
            let fileName = 'out.csv';
            const csvWriter = createCsvWriter({
              path: fileName,
              fieldDelimiter : ';',
              header: [
                {id: 'timestamp', title: 'Timestamp'},
                {id: 'pressure', title: 'Pressure'},
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
              commandObject['timestamp'] = data.readUInt32LE(4);
              commandObject['pressure'] = Math.round(data.readFloatLE(8) * 10000) / 10000;
              commandObject['temperature'] = Math.round(data.readFloatLE(12) * 10000) / 10000;
              commandObject['acc_x'] = Math.round(data.readFloatLE(16) * 10000) / 10000;
              commandObject['acc_y'] = Math.round(data.readFloatLE(20) * 10000) / 10000;
              commandObject['acc_z'] = Math.round(data.readFloatLE(24) * 10000) / 10000;
              commandObject['calcedAlt'] = calcAlt(commandObject['pressure'] * 10, commandObject['temperature']);
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