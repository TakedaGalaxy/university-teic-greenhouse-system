#include "./main.hpp"
#include "printf.h"

void logicSerialControl(RF24 *radio)
{
  if (!Serial.available())
    return;

  char c = Serial.read();

  if (c == 'R')
  {
    auto packgeReadSerialized = generateReadPackge(0, 0, 0, 4);

    if (!isFreeToSendPackge(radio))
    {
      printf("Not free to send\n");
      printf("@NOT_FREE");
      return;
    }

    sendPackge(radio, packgeReadSerialized);

    printf("Read packge sent\n");
    printf("@SENT\n");
    return;
  }

  if (c == 'W')
  {
    uint8_t bytes[16] = {0};
    auto packgeWriteSerialized = generateWritePackge(0, 0, 0, 4, 0, 0, bytes);

    if (!isFreeToSendPackge(radio))
    {
      printf("Not free to send\n");
      printf("@NOT_FREE");
      return;
    }

    sendPackge(radio, packgeWriteSerialized);

    printf("Write packge sent\n");
    printf("@SENT\n");

    return;
  }

  printf("Command not found !\n");
  printf("@NOT_FOUND_COMMAND\n");
}

void logicReadPackges(RF24 *radio)
{
  if (!isPackAvailable(radio))
    return;

  printf("------------\n");
  printf("Recived packge !\n");
  PackgeSerialized packgeSerialized = readPackge(radio);

  if (!doCheckSum(packgeSerialized))
    return;

  printf("Packge checkSum ok !\n");

  auto packge = parsePackge(packgeSerialized);

  if (!isPackgeValid(packge))
    return;

  printf("Packge is valid !\n");
}

int main()
{
  init();

  Serial.begin(115200);
  printf_begin();

  auto radio = configRF24Transmiter(7, 8);

  while (1)
  {
    logicSerialControl(radio);

    logicReadPackges(radio);
  }
}
