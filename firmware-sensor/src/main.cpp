#include "main.hpp"
#include "printf.h"

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

  if (!isPackgeMine(packge, 0, 0))
    return;

  printf("Packge is mine !\n");

  switch (packge.type)
  {
  case PACKGE_TYPE_WRITE:
    printf("Packge type is write !\n");

    writeMemoryProtocol(memory, 0, packge.data, packge.numBytes);

    printf("packge data >");
    for (int i = 0; i < packge.numBytes; i++)
      printf("%u ", packge.data[i]);
    printf("<\n");

    break;

  case PACKGE_TYPE_READ:
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
      /*
      printf("Update Memory ! %lu\n", contador);
      writeMemoryProtocol(&memory, 0, contador);
      */

      printf("MemoryProtocol >");
      for (int i = 0; i < memory.size; i++)
        printf("%u%c", memory.data[i], i < memory.size - 1 ? ' ' : '<');

      printf("\n");
    }
  }

  return 0;
}
