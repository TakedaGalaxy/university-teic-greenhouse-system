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
  packge.appId = packgeSerialized.data[1];
  packge.deviceId[0] = packgeSerialized.data[2];
  packge.deviceId[1] = packgeSerialized.data[3];
  packge.address[0] = packgeSerialized.data[4];
  packge.address[1] = packgeSerialized.data[5];
  packge.address[2] = packgeSerialized.data[6];
  packge.numBytes = (packgeSerialized.data[7] & 0xF0) >> 4;
  packge.op = packgeSerialized.data[7] & 0xF;
  packge.mask = packgeSerialized.data[8];
  packge.protocolNumber = packgeSerialized.data[9];

  memcpy(packge.data, packgeSerialized.data + 15, sizeof(uint8_t) * 16);

  packge.checkSum = packgeSerialized.data[31];

  return packge;
}

PackgeSerialized serializePackge(Packge packge)
{
  PackgeSerialized packgeSerialized;

  packgeSerialized.data[0] = packge.type;
  packgeSerialized.data[1] = packge.appId;
  packgeSerialized.data[2] = packge.deviceId[0];
  packgeSerialized.data[3] = packge.deviceId[1];
  packgeSerialized.data[4] = packge.address[0];
  packgeSerialized.data[5] = packge.address[1];
  packgeSerialized.data[6] = packge.address[2];

  packgeSerialized.data[7] = (packge.numBytes << 4) | packge.op;
  packgeSerialized.data[8] = packge.mask;
  packgeSerialized.data[9] = packge.protocolNumber;

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

