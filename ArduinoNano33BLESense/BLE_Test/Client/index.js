const noble = require('noble-uwp');
const readline = require('readline');

if (noble.state === "poweredOn") {
  noble.startScanning();
}

noble.on("stateChange", function(state) {
  console.log("BLE State change: " + state);
  if (state === "poweredOn") {
    noble.startScanning();
  } else {
    noble.stopScanning();
  }
});

noble.on('discover', function(peripheral) {
  console.log('peripheral with ID ' + peripheral.id + ' found');
  let advertisement = peripheral.advertisement;
  let localName = advertisement.localName;

  if (peripheral.id == "f1e2c75cb032") {
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
      let logger = characteristics.find(f => f.uuid == "2a3d");
      let command = characteristics.find(f => f.uuid == "19b10001e8f2537e4f6cd104768a1214");



      let cont = () => {
        repl(rl, characteristics, callback); //Calling this function again to ask new question
      };

      let writeCommand = (c, cb) => {
        let buffer = Buffer.from(c, 'ascii');
        command.write(buffer, false, cb);
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
            logger.on('data', (data, isNotification) => {
              console.log(data);
            });

            logger.subscribe((error) => {
              console.log("notify func" + error);
              
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