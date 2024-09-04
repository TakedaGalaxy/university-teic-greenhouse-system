#pragma once

#include <Arduino.h>

#define PACKGE_PROTOCOL_NUMBER 9
#define PACKGE_TYPE_SIGNAL 0b00000001
#define PACKGE_TYPE_WRITE 0b00000010
#define PACKGE_TYPE_READ 0b00000011
#define PACKGE_TYPE_OK 0b00000100
#define PACKGE_TYPE_FAIL 0b00000101

struct Packge
{
  uint8_t type = 0;
  uint8_t appId = 0;
  uint16_t deviceId = 0;
  uint32_t address = 0;
  uint8_t numBytes = 0;
  uint8_t op = 0;
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

struct MemoryProtocol
{
  uint8_t *data = nullptr;
  uint32_t size;

  ~MemoryProtocol()
  {
    delete[] data;
  }
};

uint8_t generateCheckSum(PackgeSerialized packgeSerialized);

bool doCheckSum(PackgeSerialized packgeSerialized);

Packge parsePackge(PackgeSerialized packgeSerialized);

PackgeSerialized serializePackge(Packge packge);

PackgeSerialized generateSignalPackge();

PackgeSerialized generateWritePackge(uint8_t appId, uint16_t deviceId, uint32_t address, uint8_t numBytes, uint8_t op, uint8_t mask, uint8_t bytes[16]);

MemoryProtocol alocMemoryProtocol(uint32_t size);

bool isPackgeValid(Packge packge);

bool isPackgeMine(Packge packge, uint8_t appId, uint16_t deviceId);

void writeMemoryProtocol(MemoryProtocol *memory, uint32_t address, uint8_t *data, uint8_t numBytes);

template <typename T>
void writeMemoryProtocol(MemoryProtocol *memory, uint32_t address, T value)
{
  memcpy(memory->data + address, &value, sizeof(T));
}

template <typename T>
T readMemoryProtocol(MemoryProtocol *memory, uint32_t address)
{
  T value;
  memcpy(&value, memory->data + address, sizeof(T));
  return value;
}

uint8_t *readMemoryProtocol(MemoryProtocol *memory, uint32_t address);

PackgeSerialized generateOkPackge(uint8_t appId, uint16_t deviceId, uint32_t address, uint8_t numBytes, uint8_t bytes[16]);

PackgeSerialized generateReadPackge(uint8_t appId, uint16_t deviceId, uint32_t address, uint8_t numBytes, uint8_t op = 0, uint8_t mask = 0);
