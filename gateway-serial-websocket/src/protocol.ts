export class Package {
  private static readonly PACKGE_PROTOCOL_NUMBER: number = 9; // Defina o número do protocolo conforme necessário
  private buffer: ArrayBuffer;
  private view: DataView;

  constructor(data?: Uint8Array) {
    // Cria um buffer de 32 bytes
    this.buffer = new ArrayBuffer(32);
    this.view = new DataView(this.buffer);

    // Se dados forem fornecidos, copia-os para o buffer
    if (data) {
      const length = Math.min(data.length, 32);
      new Uint8Array(this.buffer).set(data.slice(0, length));
      return;
    }
    // Caso contrário, inicializa o buffer com zeros e define o valor padrão para protocolNumber
    new Uint8Array(this.buffer).fill(0);
    this.protocolNumber = Package.PACKGE_PROTOCOL_NUMBER;
  }

  // Métodos para acessar e modificar os campos da estrutura

  get type(): number {
    return this.view.getUint8(0);
  }

  set type(value: number) {
    this.view.setUint8(0, value);
  }

  get protocolNumber(): number {
    return this.view.getUint8(1);
  }

  set protocolNumber(value: number) {
    this.view.setUint8(1, value);
  }

  get appId(): number {
    return this.view.getUint8(2);
  }

  set appId(value: number) {
    this.view.setUint8(2, value);
  }

  get deviceId(): number {
    return this.view.getUint16(3, true); // true para little-endian
  }

  set deviceId(value: number) {
    this.view.setUint16(3, value, true); // true para little-endian
  }

  get address(): number {
    return (this.view.getUint8(5) << 16) | this.view.getUint16(6, true); // true para little-endian
  }

  set address(value: number) {
    this.view.setUint8(5, value >> 16 & 0xFF);
    this.view.setUint16(6, value & 0xFFFF, true); // true para little-endian
  }

  get op(): number {
    return this.view.getUint8(8);
  }

  set op(value: number) {
    this.view.setUint8(8, value);
  }

  get mask(): number {
    return this.view.getUint8(9);
  }

  set mask(value: number) {
    this.view.setUint8(9, value);
  }

  get numBytes(): number {
    return this.view.getUint8(10);
  }

  set numBytes(value: number) {
    this.view.setUint8(10, value);
  }

  get data(): Uint8Array {
    return new Uint8Array(this.buffer, 15, 16);
  }

  get dataArrayBuffer(): ArrayBuffer{
    return this.buffer.slice(15,15+16);
  }

  set data(value: Uint8Array) {
    if (value.length > 16) {
      throw new Error("Size is more than 16 bytes.");
    }

    new Uint8Array(this.buffer, 15, 16).set(value);
  }

  get checkSum(): number {
    return this.view.getUint8(31);
  }

  set checkSum(value: number) {
    this.view.setUint8(31, value);
  }

  // Método para obter o buffer completo
  getBuffer(): ArrayBuffer {
    return this.buffer;
  }

  print() {
    console.log('Type:', this.type);
    console.log('ProtocolNumber:', this.protocolNumber);
    console.log('AppId:', this.appId);
    console.log('DeviceId:', this.deviceId);
    console.log('Address:', this.address);
    console.log('NumBytes:', this.numBytes);
    console.log('Op:', this.op);
    console.log('Mask:', this.mask);
    console.log('Data:', this.data);
    console.log('Checksum:', this.checkSum);
  }
}

export function uint32ToUint8Array(value: number): Uint8Array {
  // Cria um array de 4 bytes (32 bits) para armazenar o valor
  const bytes = new Uint8Array(4);

  if (value < 0 || value > 0xFFFFFFFF) {
    bytes.fill(0);
    return bytes;
  }

  // Preenche o array com o valor
  bytes[0] = (value >> 24 & 0xFF);    // byte mais significativo
  bytes[1] = (value >> 16 & 0xFF);    // próximo byte
  bytes[2] = (value >> 8 & 0xFF);     // próximo byte
  bytes[3] = (value & 0xFF);          // byte menos significativo

  return bytes;
}