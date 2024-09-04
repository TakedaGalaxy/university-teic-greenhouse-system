/*
import ServerProtocol, { SerialProtocol } from "./protocol/server";

const server = new ServerProtocol();

server.start();

const serial = new SerialProtocol("COM3", 115200);

serial.connect();





const pkg = new Package(inputData);

console.log('Type:', pkg.type);
console.log('AppId:', pkg.appId);
console.log('DeviceId:', pkg.deviceId);
console.log('Address:', pkg.address);
console.log('NumBytes:', pkg.numBytes);
console.log('Op:', pkg.op);
console.log('Mask:', pkg.mask);
console.log('ProtocolNumber:', pkg.protocolNumber);
console.log('Data:', pkg.data);
console.log('Checksum:', pkg.checkSum);

console.log('Buffer:', new Uint8Array(pkg.getBuffer()));
*/

import { SerialPort } from 'serialport';
import { ReadlineParser } from '@serialport/parser-readline';
import { WebSocketServer, WebSocket } from 'ws';
import { Package, uint32ToUint8Array } from './protocol';

interface Device { }

abstract class Aplication {
  appId: number;
  devices: Array<number> = [];

  constructor(appId: number) {
    this.appId = appId;
  }

  addDevice(deviceId: number) {

    if (this.devices.includes(deviceId)) {
      console.log(`[APP-${this.appId}]Device already registered !`);
      return;
    }

    this.devices.push(deviceId)
  }

  removeDevice(deviceId: number) {

    if (!this.devices.includes(deviceId)) {
      console.log(`[APP-${this.appId}]Device not found !`);
      return;
    }
    this.devices = this.devices.filter((id) => id !== deviceId);
  }

  async updateStates(sendPackge: () => Promise<boolean>) {



  }

  packgeHandler(packge: Package) { }
};


const aplications: Map<number, Map<number, WebSocket>> = new Map();

export class SerialProtocol {
  port: SerialPort;
  callBackOnConnect: () => void = () => { };

  constructor(path: string, baudRate: number) {
    this.port = new SerialPort({
      path,
      baudRate,
      autoOpen: false,
    });

    const parser = this.port.pipe(new ReadlineParser({ encoding: "ascii" }));

    parser.on('data', (data: Buffer) => this.readData(data));
  }

  connect() {
    this.port.open((err) => {
      if (err) {

        setTimeout(() => {
          this.connect();
        }, 1000);

        console.error('[SERIAL] Erro ao abrir a porta:', err.message);

        return;
      }

      console.log('[SERIAL] Porta serial aberta');
      this.callBackOnConnect();
    });

    this.port.addListener("close", () => { console.log("[SERIAL] CLOSE") })
    this.port.addListener("error", () => { console.log("[SERIAL] ERROR") })
  }

  isConnect(): boolean {
    return this.port.isOpen
  }

  readData(data: Buffer) {

    // Identificando que é um comando de controle
    if (data.at(0)?.toString() === "@") {

      const dataStr = data.toString("ascii");

      if (dataStr === "@NOT_FOUND_COMMAND") {
        console.log("[SERIAL] COMMAND NOT FOUND");
        return;
      }

      const [command, packgeStr] = dataStr.split("-");

      if (command === "@SENT") {
        console.log("[SERIAL] SENT");

        const numArray = packgeStr.split(",").map(numStr => parseInt(numStr, 16));

        const packgeSerialized = new Uint8Array(numArray);

        const packageProtocol = new Package(packgeSerialized);

        if (packageProtocol.appId === 0) {

          const list = aplications.get(0);

          if (list === undefined) return;

          let variable = "";

          if (packageProtocol.address === 0) {
            variable = "COUNTER";
          }

          let command = "R"
          if (packageProtocol.type === 0b00000010) {
            command = "W";
          }

          list.forEach((ws) => {
            ws.send(JSON.stringify({
              device: packageProtocol.deviceId,
              variable,
              command,
              status: "SENT"
            }))
          });
        }

        return;
      }

      if (command === "@OK") {
        console.log("[SERIAL] OK");

        const numArray = packgeStr.split(",").map(numStr => parseInt(numStr, 16));

        const packgeSerialized = new Uint8Array(numArray);

        const packageProtocol = new Package(packgeSerialized);

        if (packageProtocol.appId === 0) {
          const list = aplications.get(0);

          if (list === undefined) return;

          let variable = "";

          if (packageProtocol.address === 0) {
            variable = "COUNTER";
          }

          if (packageProtocol.numBytes === 0) {
            list.forEach((ws) => {
              ws.send(JSON.stringify({
                device: packageProtocol.deviceId,
                variable,
                command: "W",
                status: "OK"
              }))
            });
            return;
          }

          let value = 0;

          if (variable === "COUNTER") {
            const view = new DataView(packageProtocol.dataArrayBuffer);

            value = view.getUint32(0, true);
          }

          list.forEach((ws) => {
            ws.send(JSON.stringify({
              device: packageProtocol.deviceId,
              variable,
              command: "R",
              status: "OK",
              value
            }))
          });
        }

        return;
      }

      if (dataStr === "@NOT_FREE") {
        console.log("[SERIAL] NOR FREE");

        const numArray = packgeStr.split(",").map(numStr => parseInt(numStr, 16));

        const packgeSerialized = new Uint8Array(numArray);

        const packageProtocol = new Package(packgeSerialized);

        if (packageProtocol.appId === 0) {
          const list = aplications.get(0);

          if (list === undefined) return;

          let variable = "";

          if (packageProtocol.address === 0) {
            variable = "COUNTER";
          }

          let command = "R"
          if (packageProtocol.type === 0b00000010) {
            command = "W";
          }

          list.forEach((ws) => {
            ws.send(JSON.stringify({
              device: packageProtocol.deviceId,
              variable,
              command,
              status: "NOT_FREE"
            }))
          });
        }
        return;
      }

      return;
    }

    // Apenas log
    console.log(`[SERIAL] LOG (${data.toString("ascii")})`);
  }

  async sendPackge(packge: Package) {
    console.log("[SERIAL] Send Packge");

    if (packge.type === 0b00000010) {
      this.port.write("W", (err) => err && console.error("[SERIAL] Error ", err));
      this.port.write(new Uint8Array(packge.getBuffer()), (err) => err && console.error("[SERIAL] Error ", err));
    }

    if (packge.type === 0b00000011) {
      this.port.write("R", (err) => err && console.error("[SERIAL] Error ", err));
      this.port.write(new Uint8Array(packge.getBuffer()), (err) => err && console.error("[SERIAL] Error ", err));
    }
  }
}
const serial = new SerialProtocol("COM8", 115200);
serial.connect();

const wss = new WebSocketServer({ port: 4000 });
const packgesToSend: Array<Package> = [];

//Aplicação 0
aplications.set(0, new Map());

wss.on('connection', (ws: WebSocket) => {
  console.log("[WEBSOCKET] New Client");

  const id = new Date().getTime();
  let listing = false;
  let appIdClient = 0;

  ws.on('message', (message: string) => {
    const { appId, command, device, variable, value } = JSON.parse(message.toString())

    switch (command) {
      case "LISTEN":
        const list = aplications.get(appId);

        if (list === undefined) {
          console.log("[WEBSOCKET] Not found !");
          ws.close();
          return;
        }

        list.set(id, ws);

        // Atualizando a lista de websocket para aplicação
        aplications.set(appId, list);
        listing = true;
        appIdClient = appId;
        console.log(`[WEBSOCKET] ${id} is listing`);
        break;
      case "W":

        if (!serial.isConnect()) {
          ws.send(JSON.stringify({
            command,
            device,
            variable,
            result: "SERIAL_ERROR"
          }))
          return;
        }

        console.log("[WEBSOCKET]: WRITE", device, variable, value);

        if (appIdClient === 0 && variable === "COUNTER") {

          const packge = new Package();

          packge.type = 0b00000010;
          packge.appId = appId;
          packge.deviceId = device;
          packge.numBytes = 4;
          packge.data = uint32ToUint8Array(value);

          //serial.sendPackge(packge);

          packgesToSend.push(packge);
        }

        break
      default:
        ws.close();
    }
  });

  ws.on('close', () => {
    console.log("[WEBSOCKET] Close client !");

    if (listing) {
      const list = aplications.get(appIdClient);

      if (list === undefined) return;

      list.delete(id);
    }
  });
});

console.log("[WEBSOCKET] On localhost:4000");

const appsRuning = [
  { appId: 0, devicesId: [0, 1] }
];

function readMemorys() {
  setTimeout(() => {
    appsRuning.forEach(({ appId, devicesId }) => {
      if (appId === 0) {
        devicesId.forEach((id) => {
          const packgeCounter = new Package();

          // Lendo varivel counter
          packgeCounter.type = 0b00000011;
          packgeCounter.appId = appId;
          packgeCounter.deviceId = id;
          packgeCounter.address = 0;
          packgeCounter.numBytes = 4;

          packgesToSend.push(packgeCounter);
        });
      }
    });

    readMemorys();
  }, 500);
}

readMemorys();

setInterval(() => {

  const packge = packgesToSend.shift();

  if (packge === undefined) return;

  if (serial.isConnect()) {
    serial.sendPackge(packge);
  }

}, 200);