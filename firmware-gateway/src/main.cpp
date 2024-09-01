#include "./main.hpp"
#include "printf.h"

int main()
{
  init();

  Serial.begin(115200);
  printf_begin();

  auto radio = new RF24(7, 8);

  radio->begin();
  radio->setAutoAck(false);
  radio->setRetries(0, 0);
  radio->setPALevel(RF24_PA_MIN);

  uint8_t goodChannel = getGoodChannel(radio);

  radio->setChannel(goodChannel);

  printf("Good Channel: %u\n", goodChannel);

  uint32_t lastTimeSendSignal = 0;

  while (1)
  {
    uint32_t currentTime = millis();

    if (currentTime - lastTimeSendSignal >= 100)
    {
      lastTimeSendSignal = currentTime;
      sendSignal(radio, goodChannel);
      printf("SIGNAL SEND : CHANNEL %u\n", goodChannel);
    }
  }
}
