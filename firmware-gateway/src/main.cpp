#include "./main.hpp"
#include "printf.h"

int main()
{
  init();

  Serial.begin(115200);
  printf_begin();

  auto radio = new RF24(7, 8);

  radio->begin();
  // radio->setAutoAck(false);
  // radio->setRetries(0, 0);
  radio->setPALevel(RF24_PA_MIN);
  radio->setDataRate(RF24_2MBPS);

  // uint8_t goodChannel = getGoodChannel(radio);
  uint8_t goodChannel = 90;

  radio->setChannel(goodChannel);

  printf("Good Channel: %u\n", goodChannel);

  uint32_t lastTimeSendSignal = 0;

  while (1)
  {
    uint32_t currentTime = millis();

    if (currentTime - lastTimeSendSignal >= 500)
    {
      lastTimeSendSignal = currentTime;
      // sendSignal(radio, goodChannel);
      const byte address[6] = "pack ";
      radio->stopListening();
      radio->openWritingPipe(address);

      uint8_t data[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
      auto packgeSerial = generateWritePackge(0, 0, 0, 16, 0, 0, data);
      
      radio->write(packgeSerial.data, packgeSerial.size);

      printf("SIGNAL SEND : CHANNEL %u\n", goodChannel);
    }

    if (Serial.available())
    {
      char startByte = Serial.read();
    }
  }
}
