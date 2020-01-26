export default class DataPointParser {
  constructor() {

  }

  parse(value) {
    let offset = 0;

    let result = {
      state: value.getUint16(0 + offset, true),
      timestamp: value.getUint32(4 + offset, true),
      pressure: value.getFloat32(8 + offset, true),
      temperature: value.getFloat32(12 + offset, true),
      accX: value.getFloat32(16 + offset, true),
      accY: value.getFloat32(20 + offset, true),
      accZ: value.getFloat32(24 + offset, true),
      pressureDelta: value.getFloat32(28 + offset, true),
      pressureDeltaMid: value.getFloat32(32 + offset, true),
      altitude: value.getFloat32(36 + offset, true),
    };

    return result;
  }
}