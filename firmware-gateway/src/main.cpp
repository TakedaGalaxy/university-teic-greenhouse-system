#include "./main.hpp"
#include "printf.h"

void logicSerialControl(RF24 *radio)
{
  if (!Serial.available())
    return;

  char c = Serial.read();

  if (c == 'R')
  {
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
    return;
  }
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
  }
}
