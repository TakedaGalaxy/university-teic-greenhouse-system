#include "./protocol.hpp"

uint8_t generateCheckSum(PackgeSerialized packgeSerialized)
{
  uint8_t checkSum = 0;

  for (int i = 0; i < 31; i++)
    checkSum += packgeSerialized.data[i];

  return ~checkSum + 1;
}

bool doCheckSum(PackgeSerialized packgeSerialized)
{
  uint8_t checkSumGenerated = generateCheckSum(packgeSerialized);

  return checkSumGenerated == packgeSerialized.data[31];
}

Packge parsePackge(PackgeSerialized packgeSerialized)
{
  Packge packge;

  packge.type = packgeSerialized.data[0];
  packge.protocolNumber = packgeSerialized.data[1];
  packge.appId = packgeSerialized.data[2];
  packge.deviceId = (packgeSerialized.data[3] << 8) | packgeSerialized.data[4];
  packge.address = ((uint32_t)packgeSerialized.data[5] << 16) | ((uint32_t)packgeSerialized.data[6] << 8) | (uint32_t)packgeSerialized.data[7];
  packge.op = packgeSerialized.data[8];
  packge.mask = packgeSerialized.data[9];
  packge.numBytes = packgeSerialized.data[10];

  memcpy(packge.data, packgeSerialized.data + 15, sizeof(uint8_t) * 16);

  packge.checkSum = packgeSerialized.data[31];

  return packge;
}

PackgeSerialized serializePackge(Packge packge)
{
  PackgeSerialized packgeSerialized;

  packgeSerialized.data[0] = packge.type;
  packgeSerialized.data[1] = packge.protocolNumber;
  packgeSerialized.data[2] = packge.appId;
  packgeSerialized.data[3] = packge.deviceId >> 8;
  packgeSerialized.data[4] = packge.deviceId & 0xff;
  packgeSerialized.data[5] = packge.address >> 16;
  packgeSerialized.data[6] = (packge.address >> 8) & 0xff;
  packgeSerialized.data[7] = packge.address & 0xff;
  packgeSerialized.data[8] = packge.op;
  packgeSerialized.data[9] = packge.mask;
  packgeSerialized.data[10] = packge.numBytes;

  memcpy(packgeSerialized.data + 15, packge.data, sizeof(uint8_t) * 16);

  packgeSerialized.data[31] = generateCheckSum(packgeSerialized);

  return packgeSerialized;
}

PackgeSerialized generateSignalPackge()
{
  Packge packge;

  packge.type = PACKGE_TYPE_SIGNAL;
  packge.protocolNumber = PACKGE_PROTOCOL_NUMBER;

  return serializePackge(packge);
}

PackgeSerialized generateWritePackge(uint8_t appId, uint16_t deviceId, uint32_t address, uint8_t numBytes, uint8_t op, uint8_t mask, uint8_t bytes[16])
{
  Packge packge;
  packge.type = PACKGE_TYPE_WRITE;
  packge.protocolNumber = PACKGE_PROTOCOL_NUMBER;
  packge.appId = appId;
  packge.deviceId = deviceId;
  packge.address = address;
  packge.numBytes = numBytes;
  packge.op = op;
  packge.mask = mask;
  memcpy(packge.data, bytes, sizeof(uint8_t) * numBytes);

  return serializePackge(packge);
}

MemoryProtocol alocMemoryProtocol(uint32_t size)
{
  MemoryProtocol memory;

  memory.data = new uint8_t[size];
  memory.size = size;

  for (uint32_t i = 0; i < size; i++)
    memory.data[i] = 0;

  return memory;
}

bool isPackgeValid(Packge packge)
{
  return packge.protocolNumber == PACKGE_PROTOCOL_NUMBER;
}

bool isPackgeMine(Packge packge, uint8_t appId, uint16_t deviceId)
{
  return packge.appId == appId && packge.deviceId == deviceId;
}

void writeMemoryProtocol(MemoryProtocol *memory, uint32_t address, uint8_t *data, uint8_t numBytes)
{
  memcpy(memory->data + address, data, sizeof(uint8_t) * numBytes);
}

PackgeSerialized generateOkPackge(uint8_t appId, uint16_t deviceId, uint32_t address, uint8_t numBytes, uint8_t bytes[16])
{
  Packge packge;
  packge.type = PACKGE_TYPE_OK;
  packge.protocolNumber = PACKGE_PROTOCOL_NUMBER;
  packge.appId = appId;
  packge.deviceId = deviceId;
  packge.address = address;
  packge.numBytes = numBytes;
  packge.op = 0;
  packge.mask = 0;

  if (bytes != nullptr)
    memcpy(packge.data, bytes, sizeof(uint8_t) * 16);

  return serializePackge(packge);
}

uint8_t *readMemoryProtocol(MemoryProtocol *memory, uint32_t address)
{
  return memory->data + address;
}

PackgeSerialized generateReadPackge(uint8_t appId, uint16_t deviceId, uint32_t address, uint8_t numBytes, uint8_t op, uint8_t mask)
{
  Packge packge;
  packge.type = PACKGE_TYPE_READ;
  packge.protocolNumber = PACKGE_PROTOCOL_NUMBER;
  packge.appId = appId;
  packge.deviceId = deviceId;
  packge.address = address;
  packge.numBytes = numBytes;
  packge.op = op;
  packge.mask = mask;

  return serializePackge(packge);
}