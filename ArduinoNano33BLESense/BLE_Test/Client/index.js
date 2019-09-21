const noble = require('noble');
const readline = require('readline');

noble.on('stateChange', function(state) {
  if (state === 'poweredOn') {
    console.log("Starting scan...");
    noble.startScanning(undefined, true);
  } else {
    console.log("Error:" + state + " stopping...");
    noble.stopScanning();
  }
});
noble.on('discover', function(peripheral) {
  console.log('peripheral with ID ' + peripheral.id + ' found');
  let advertisement = peripheral.advertisement;
  let localName = advertisement.localName;

  if (localName == "AdAstra Telemetry") {
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
  console.log("exit,     e               exits interactive mode and returns to BT search");
  console.log("count,    c               current count of datapoints");
  console.log("reset,    r               reset datalogger");
  console.log("download, d               download data");
  peripheral.connect(function(error) {
    peripheral.discoverAllServicesAndCharacteristics(function(error, services, characteristics) {
      repl(rl, characteristics, () => {
        peripheral.disconnect();
        callback();
      });
    });
  });
}

function repl(rl, characteristics, callback) {

      //console.log(characteristics);
      let count = characteristics.find(f => f.uuid == "2ac0");
      let logger = characteristics.find(f => f.uuid == "2A3D");
      let command = characteristics.find(f => f.uuid == "19B10001-E8F2-537E-4F6C-D104768A1214");

      let cont = () => {
        repl(rl, characteristics, callback); //Calling this function again to ask new question
      };

      rl.question('Command: ', function (answer) {
        switch (answer) {
          case 'exit':
          case 'e':
            rl.close();
            callback();
            break;
          case 'reset':
          case 'r':
            cont();
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
            cont();
            break;
          default:
            console.log(`Unknown command: ${answer}`)
            cont();
            break;
        }
  });
}