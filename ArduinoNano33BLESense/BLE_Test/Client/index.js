var noble = require('noble-uwp');
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

  repl(rl, peripheral, callback);
}

function repl(rl, peripheral, callback) {
  peripheral.connect(function(error) {
    peripheral.discoverServices([], function(error, services) {
      console.log(JSON.stringify(services));

      rl.question('Command: ', function (answer) {
        switch (answer) {
          case 'exit':
          case 'e':
            rl.close();
            callback();
          case 'reset':
          case 'r':
            break;
          case 'download':
          case 'd':

          default:
            console.log(`Unknown command: ${answer}`)
            break;
        }
        repl(rl, peripheral, callback); //Calling this function again to ask new question
      });
    });
  });
}