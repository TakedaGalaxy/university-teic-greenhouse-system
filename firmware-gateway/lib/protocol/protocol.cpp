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
  packge.deviceId[0] = packgeSerialized.data[3];
  packge.deviceId[1] = packgeSerialized.data[4];
  packge.address[0] = packgeSerialized.data[5];
  packge.address[1] = packgeSerialized.data[6];
  packge.address[2] = packgeSerialized.data[7];
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
  packgeSerialized.data[3] = packge.deviceId[0];
  packgeSerialized.data[4] = packge.deviceId[1];
  packgeSerialized.data[5] = packge.address[0];
  packgeSerialized.data[6] = packge.address[1];
  packgeSerialized.data[7] = packge.address[2];

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
  packge.deviceId[0] = (deviceId & 0xff00) >> 8;
  packge.deviceId[1] = deviceId & 0xff;
  packge.address[0] = (address & 0xff0000) >> 16;
  packge.address[1] = (address & 0xff0000) >> 8;
  packge.address[2] = address & 0xff;
  packge.numBytes = numBytes;
  packge.op = op;
  packge.mask = mask;
  memcpy(packge.data, bytes, sizeof(uint8_t) * 16);

  return serializePackge(packge);
}