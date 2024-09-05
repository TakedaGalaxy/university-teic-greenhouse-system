#include "./main.hpp"
#include "printf.h"

void logicSerialControl(RF24 *radio)
{
  if (!Serial.available())
    return;

  char c = Serial.read();

  PackgeSerialized packgeSerialized;

  if (c == 'R' || c == 'W')
  {
    Serial.readBytes(packgeSerialized.data, packgeSerialized.size);

    packgeSerialized.data[31] = generateCheckSum(packgeSerialized);

    Packge packge = parsePackge(packgeSerialized);

    if (!isPackgeValid(packge))
    {
      printf("Packge is not valid\n");
      printf("@NOT_VALID\n");
      return;
    }
  }

  if (c == 'R')
  {
    bool isFree = false;

    for (int i = 0; i < 5; i++)
    {
      isFree = isFreeToSendPackge(radio);
      if (isFree)
        break;
      delay(10);
    }

    if (!isFree)
    {
      printf("Not free to send\n");
      printf("@NOT_FREE-");
      for (int i = 0; i < packgeSerialized.size; i++)
        printf("%.2x%c", packgeSerialized.data[i], i < packgeSerialized.size - 1 ? ',' : '\n');
      return;
    }

    sendPackge(radio, packgeSerialized);

    printf("Read packge sent\n");
    printf("@SENT-");
    for (int i = 0; i < packgeSerialized.size; i++)
      printf("%.2x%c", packgeSerialized.data[i], i < packgeSerialized.size - 1 ? ',' : '\n');

    /*
    // Apagar apenas para teste
    printf("@OK-");
    uint8_t data[16] = {0};
    uint32_t valor = 901000;
    memcpy(data, &valor, sizeof(valor));
    auto packgeOk = generateOkPackge(0, 0, 0, 4, data);
    for (int i = 0; i < packgeOk.size; i++)
      printf("%.2x%c", packgeOk.data[i], i < packgeOk.size - 1 ? ',' : '\n');
    */
    return;
  }

  if (c == 'W')
  {
    uint8_t bytes[16] = {0};
    // auto packgeWriteSerialized = generateWritePackge(0, 0, 0, 4, 0, 0, bytes);
    bool isFree = false;

    for (int i = 0; i < 5; i++)
    {
      isFree = isFreeToSendPackge(radio);
      if (isFree)
        break;
      ;
      delay(10);
    }

    if (!isFree)
    {
      printf("Not free to send\n");
      printf("@NOT_FREE-");
      for (int i = 0; i < packgeSerialized.size; i++)
        printf("%.2x%c", packgeSerialized.data[i], i < packgeSerialized.size - 1 ? ',' : '\n');
      return;
    }

    sendPackge(radio, packgeSerialized);

    printf("Write packge sent\n");
    printf("@SENT-");
    for (int i = 0; i < packgeSerialized.size; i++)
      printf("%.2x%c", packgeSerialized.data[i], i < packgeSerialized.size - 1 ? ',' : '\n');

    /*
    // Apagar apenas para teste
    printf("@OK-");
    auto packgeOk = generateOkPackge(0, 0, 0, 0, nullptr);
    for (int i = 0; i < packgeOk.size; i++)
      printf("%.2x%c", packgeOk.data[i], i < packgeOk.size - 1 ? ',' : '\n');
    */
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

  if(packge.type != PACKGE_TYPE_OK) return;

  printf("@OK-");
  for (int i = 0; i < packgeSerialized.size; i++)
    printf("%.2x%c", packgeSerialized.data[i], i < packgeSerialized.size - 1 ? ',' : '\n');
}

int main()
{
  init();

  Serial.begin(115200);
  printf_begin();

  auto radio = configRF24Transmiter(7, 8);

  printf("@START\n");

  while (1)
  {
    logicSerialControl(radio);

    logicReadPackges(radio);
  }
}
