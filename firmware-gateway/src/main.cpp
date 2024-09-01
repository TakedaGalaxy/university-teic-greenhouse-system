#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

#define NUM_CHANNELS 128

uint8_t getGoodChannel(RF24 *radio, uint32_t timeCheck = 128)
{
  uint32_t channelsScores[NUM_CHANNELS];

  memset(channelsScores, 0, sizeof(channelsScores));

  for (uint8_t i = 0; i < NUM_CHANNELS; i++)
  {
    radio->setChannel(i);

    radio->startListening();
    delayMicroseconds(timeCheck);
    radio->stopListening();

    if (radio->testCarrier())
      channelsScores[i] += 1;
  }

  uint8_t minI = 0;

  for (uint8_t i = 1; i < NUM_CHANNELS; i++)
    if (channelsScores[i] < channelsScores[minI])
      minI = i;

  return minI;
}

bool sendSignal(RF24 *radio, uint8_t channel)
{
  uint64_t addressSignal = 0xf0f0f0f0f0ULL;

  radio->stopListening();
  radio->setChannel(channel);
  radio->openWritingPipe(addressSignal);

  const char text[32] = "1234567812345678123456781234567";
  radio->write(&text, sizeof(text));
  printf("Enviado em %d\n", channel);

  return true;
}

uint8_t findChannel(RF24 *radio, uint32_t timeout = 200)
{
  uint64_t addressSignal = 0xf0f0f0f0f0ULL;

  radio->openReadingPipe(0, addressSignal);
  radio->startListening();

  uint8_t channel = 0;

  for(int i = 0; i < NUM_CHANNELS; i++){
    radio->setChannel(i);
    

    if (radio->available(0))
    {
      char text[32] = "";
      radio->read(&text, sizeof(text));
      Serial.println(text);
      return;
    }
  }

  return 0;
}

void gatway()
{
  auto radio = new RF24(7, 8);

  radio->begin();
  radio->setAutoAck(false);
  radio->setRetries(0, 0);
  radio->setPALevel(RF24_PA_MIN);

  uint8_t goodChannel = getGoodChannel(radio);

  radio->setChannel(goodChannel);

  printf("Good Channel: %u\n", goodChannel);

  while (1)
  {
    sendSignal(radio, goodChannel);
  }
}

void sensor()
{
  auto radio = new RF24(7, 8);

  radio->begin();
  radio->setAutoAck(false);
  radio->setRetries(0, 0);
  radio->setPALevel(RF24_PA_MIN);

  uint8_t goodChannel = 4;

  radio->setChannel(goodChannel);

  printf("Good Channel: %u\n", goodChannel);

  while (1)
  {
    readSignal(radio, goodChannel);
  }
}

int main()
{
  init();

  Serial.begin(115200);
  printf_begin();

  //gatway();
  sensor();
}
