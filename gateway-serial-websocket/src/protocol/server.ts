
import express from "express"
import { SerialPort } from 'serialport';
import { ReadlineParser } from '@serialport/parser-readline';
import { WebSocketServer, WebSocket } from 'ws';

export class SerialProtocol {
  port: SerialPort;
  callBackOnData: (data: Buffer) => void = (data) => { };

  constructor(path: string, baudRate: number) {

    this.port = new SerialPort({
      path,
      baudRate,
      autoOpen: false,
    });

    const parser = this.port.pipe(new ReadlineParser());

    parser.on('data', (data: Buffer) => {
      console.log(`[SERIAL]: DATA -> ${data}`);
      this.callBackOnData(data);
    });
  }

  connect() {
    this.port.open((err) => {
      if (err) {

        setTimeout(() => {
          this.connect();
        }, 1000);

        console.error('[SERIAL]: Erro ao abrir a porta:', err.message);

        return;
      }

      console.log('[SERIAL]: Porta serial aberta');
    });
  }

  isConnect(): boolean {
    return this.port.isOpen
  }

  setOnData(callBack: (data: Buffer) => void) {
    this.callBackOnData = callBack;
  }

  sendData(data: string) {
    this.port.write(data, (err) => {
      if (err) {
        return console.error('[SERIAL]: Erro ao enviar dados ->', err.message);
      }
      console.log('[SERIAL]: Dados enviados ->', data);
    });
  }
}

export default class ServerProtocol {

  constructor() {}

  start() {
    const wss = new WebSocketServer({ port: 8080 });

    wss.on('connection', (ws: WebSocket) => {
      console.log('Cliente conectado');

      // Envia uma mensagem para o cliente assim que ele se conecta
      ws.send('Bem-vindo ao servidor WebSocket!');

      // Lida com mensagens recebidas do cliente
      ws.on('message', (message: string) => {
        console.log('Mensagem recebida do cliente:', message);

        // Envia uma resposta de volta ao cliente
        ws.send(`Mensagem recebida: ${message}`);
      });

      // Lida com a desconexão do cliente
      ws.on('close', () => {
        console.log('Cliente desconectado');
      });
    });

    console.log('Servidor WebSocket está rodando na porta 8080');
  }
}

/*
const app = express();

app.get("/", (request, response) => {
  response.status(200).json({
    status: "ONLINE"
  })
})

app.post("/set-address")

app.listen(4000, () => {
  console.log("[SERVER]: Running in http://localhost:4000");
})

// Configuração da porta serial (substitua 'COM3' pela porta serial correta)
const port = new SerialPort({
  path: 'COM8',
  baudRate: 115200,
  autoOpen: false, // Não abre automaticamente, vamos abrir manualmente
});

// Configuração do parser para ler as linhas de dados recebidas
const parser = port.pipe(new ReadlineParser());

// Abre a porta serial
port.open((err) => {
  if (err) {
    return console.error('Erro ao abrir a porta:', err.message);
  }
  console.log('Porta serial aberta');
});

// Evento para receber dados
parser.on('data', (data: string) => {
  console.log('Dados recebidos:', data);
});

// Enviar dados para a porta serial
function sendData(data: string) {
  port.write(data, (err) => {
    if (err) {
      return console.error('Erro ao enviar dados:', err.message);
    }
    console.log('Dados enviados:', data);
  });
}

// Exemplo de envio de dados
sendData('Olá, dispositivo serial!');
*/

