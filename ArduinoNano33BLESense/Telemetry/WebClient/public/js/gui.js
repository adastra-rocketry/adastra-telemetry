export default class GUI {
  constructor() {
    this.commandListeners = [];

    // Set up the controls we see on the screen    
    this.elements = {
      heading : TD.label({x:10,y:10,width:200,height:50,label:"My Dashboard"}),
      itemCount : TD.gauge({x:10,y:70,width:200,height:220,label:"Item count",value:0,min:0,max:1024}),
      state: TD.value({x:10,y:300,width:200,height:60,label:"State",value:"0"}),
      launchIdle :TD.button({x:10,y:370,width:200,height:80,label:"Go to LaunchIdle",value:0,name:"button",onchange:() => this.sendCommand('launchidle')}),
      resetButton :TD.button({x:10,y:450,width:200,height:80,label:"Reset",value:0,name:"button",onchange:() => this.sendCommand('reset')}),
      downloadButton :TD.button({x:10,y:530,width:200,height:80,label:"Download",value:0,name:"button",onchange:() => this.sendCommand('download')}),
      launchAltitude: TD.value({x:10, y: 620, height: 50, width: 200, label: "Launch Altitude (m over NN)", step: 5, value: 145, min: 0, max: 300}),
      launchPressure: TD.value({x:10, y: 670, height: 50, width: 200, label: "Launch Pressure (hpa)", step: 1, value: 1003.25, min: 1000, max: 1050}),
      setLaunchParameter :TD.button({x:10,y:720,width:200,height:80,label:"Set Launch Parameter",value:0,name:"button",onchange:() => this.sendCommand('launchparameter')}),

      
      timestamp: TD.value({x:240,y:70,width:200,height:60,label:"Timestamp",value:"0"}),
      heading2 : TD.label({x:240,y:10,width:1020,height:50,label:"Live View"}),
      temperature : TD.gauge({x:240,y:140,width:200,height:220,label:"Temperature",value:0,min:0,max:50}),
      temperatureGraph: TD.graph({gridx: 10, gridy: 10, x:450,y:140,width:400,height:220,label:"Temperature",data: []}),
      pressure : TD.gauge({x:240,y:370,width:200,height:220,label:"Pressure",value:0,min:100,max:110}),
      pressureGraph: TD.graph({gridx: 10, gridy: 10, x:450,y:370,width:400,height:220,label:"Pressure",data: []}),
      altitude : TD.gauge({x:240,y:600,width:200,height:220,label:"Altitude",value:-10,min:-10,max:300}),
      altitudeGraph: TD.graph({gridx: 10, gridy: 10, x:450,y:600,width:400,height:220,label:"Altitude",data: []}),
      
      acceleration: TD.value({x:860,y:70 ,width:400,height:60,label:"Acceleration",value:"0"}),
      accxGraph: TD.graph({gridx: 10, gridy: 1, x:860,y:140,width:400,height:100,label:"Acc X",data: []}),
      accyGraph: TD.graph({gridx: 10, gridy: 1, x:860,y:250,width:400,height:100,label:"Acc Y",data: []}),
      acczGraph: TD.graph({gridx: 10, gridy: 1, x:860,y:360,width:400,height:100,label:"Acc Z",data: []}),
      pressureDeltaGraph: TD.graph({gridx: 10, gridy: 0.1, x:860,y:470,width:400,height:220,label:"PressureDelta",data: []}),
      
      modal: TD.modal({x:10,y:10,width:1400,height:800,label:"Click to connect",onchange:() => this.sendCommand("connect")}),
    }

    for (var i in this.elements) {
      document.body.appendChild(this.elements[i]);
    }
  }

  getValue(name) {
    let element = this.elements[name];
    return element.opts.value;
  }

  setValue(name, value) {
    let element = this.elements[name];
    if(typeof element.setValue == "function") {
      element.setValue(value);
    } else {
      element.setData(value);
    }
  }

  sendCommand(command) {
    this.commandListeners.forEach((listener) => {
      listener(this, command);
    });
  }
}