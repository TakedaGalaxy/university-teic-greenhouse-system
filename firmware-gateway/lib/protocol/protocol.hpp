#pragma once

#include <Arduino.h>

#define PACKGE_PROTOCOL_NUMBER 9
#define PACKGE_TYPE_SIGNAL 0b00000001

struct Packge
{
  uint8_t type = 0;
  uint8_t appId = 0;
  uint8_t deviceId[2] = {0};
  uint8_t address[3] = {0};
  uint8_t numBytes = 0; // 4bits MSB
  uint8_t op = 0;       // 4bits LSB
  uint8_t mask = 0;
  uint8_t protocolNumber = PACKGE_PROTOCOL_NUMBER;
  uint8_t data[16] = {0};
  uint8_t checkSum = 0;
};

struct PackgeSerialized
{
  uint8_t data[32] = {0};
  uint8_t size = 32;
};

uint8_t generateCheckSum(PackgeSerialized packgeSerialized);

bool doCheckSum(PackgeSerialized packgeSerialized);

Packge parsePackge(PackgeSerialized packgeSerialized);

PackgeSerialized serializePackge(Packge packge);

PackgeSerialized generateSignalPackge();
