var noble = require('noble');
var readline = require('readline');

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
  var advertisement = peripheral.advertisement;
  var localName = advertisement.localName;

  if (localName == "AdAstra Telemetry") {
    noble.stopScanning();
    enterInteractiveMode(peripheral, () => {
      console.log("Starting scan...");
      noble.startScanning();
    });
  }
});

function enterInteractiveMode(peripheral, callback) {
  var rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
  });
  console.log("Connected to " + peripheral.advertisement.localName);
  console.log("Possible commands are:");
  console.log("exit,     e               exits interactive mode and returns to BT search");
  console.log("reset,    r               reset datalogger");
  console.log("download, d               download data");
  peripheral.connect(function(error) {
    peripheral.discoverAllServicesAndCharacteristics(function(error, services, characteristics) {
      repl(rl, characteristics, callback);
    });
  });
}

function repl(rl, characteristics, callback) {

      console.log(characteristics);
      var count = characteristics.find(f => f.uuid == "2ac0");

      function cont() {
        repl(rl, characteristics, callback); //Calling this function again to ask new question
      }

      rl.question('Command: ', function (answer) {
        switch (answer) {
          case 'exit':
          case 'e':
            rl.close();
            callback();
          case 'reset':
          case 'r':
            break;
          case 'count':
          case 'c':
              count.read((error, data) => {
                console.log(data.length)
                data = data.readInt8();
                if(error) {
                  console.log("Error while retrieving count");
                } else {
                  console.log(`There are currently ${data} datapoints in the logger.`);
                }
                cont();
              });
          case 'download':
          case 'd':
            break;
          default:
            console.log(`Unknown command: ${answer}`)
            cont();
            break;
        }
  });
}