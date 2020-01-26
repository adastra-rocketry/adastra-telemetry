import GUI from "./gui.js";
import Client from "./client.js";

export default class Init {
  constructor() {
    let gui = new GUI();
    let client = new Client(gui);
  }
}


let init = new Init();