#include "main.hpp"
#include "printf.h"

#define DEVICE_ID 1

void logicRF24Protocol(RF24 *radio, MemoryProtocol *memory)
{

  if (!isPackAvailable(radio))
    return;

  printf("--------------------\n");
  printf("Recived packge !\n");
  PackgeSerialized packgeSerialized = readPackge(radio);

  if (!doCheckSum(packgeSerialized))
    return;

  printf("Packge checkSum ok !\n");

  auto packge = parsePackge(packgeSerialized);

  if (!isPackgeValid(packge))
    return;

  printf("Packge is valid !\n");
  printf("%d\n", packge.deviceId);
  if (!isPackgeMine(packge, 0, DEVICE_ID))
    return;

  printf("Packge is mine !\n");

  switch (packge.type)
  {
  case PACKGE_TYPE_WRITE:
    printf("Packge type is write !\n");

    writeMemoryProtocol(memory, 0, packge.data, packge.numBytes);

    printf("Sending Packge OK 5 times\n");

    for (int i = 0; i < 6; i++)
    {
      auto packgeOkSerialized = generateOkPackge(0, DEVICE_ID, packge.address, 0, nullptr);

      while (1)
      {
        if (!isFreeToSendPackge(radio))
          continue;

        sendPackge(radio, packgeOkSerialized);
        printf("[%d] Ok packge sent !\n", i);
        break;
      }
      delay(40);
    }

    break;

  case PACKGE_TYPE_READ:
    printf("Packge type is read !\n");

    uint8_t *data = readMemoryProtocol(memory, packge.address);

    auto packgeOkSerialized = generateOkPackge(0, DEVICE_ID, packge.address, packge.numBytes, data);

    printf("Sending Packge OK 5 times\n");

    for (int i = 0; i < 6; i++)
    {
      while (1)
      {
        if (!isFreeToSendPackge(radio))
          continue;

        sendPackge(radio, packgeOkSerialized);
        printf("[%d] Ok packge sent !\n", i);
        break;
      }
      delay(40);
    }

    break;

  default:
    printf("Packge type not found !\n");
    break;
  }
}

int main()
{
  init();

  Serial.begin(115200);
  printf_begin();

  auto radio = configRF24Sensor(7, 8);
  auto memory = alocMemoryProtocol(50);

  uint32_t contador = 0;

  while (1)
  {
    logicRF24Protocol(radio, &memory);

    uint32_t currentTime = millis();

    if (currentTime - contador >= 1000)
    {
      contador = currentTime;

      // printf("Update Memory ! %lu\n", contador);
      /*
      uint8_t *data = readMemoryProtocol(&memory, 0);
      auto packgeOkSerialized = generateOkPackge(0, 0, 0, 4, data);
      for (int i = 0; i < 6; i++)
      {
        sendPackge(radio, packgeOkSerialized);
        delay(100);
      }
      */
      auto readValue = readMemoryProtocol<uint32_t>(&memory, 0);
      readValue += 1000;

      writeMemoryProtocol(&memory, 0, readValue);

      printf("MemoryProtocol >");
      for (size_t i = 0; i < memory.size; i++)
        printf("%u%c", memory.data[i], i < memory.size - 1 ? ' ' : '<');

      printf("\n");
    }
  }

  return 0;
}
