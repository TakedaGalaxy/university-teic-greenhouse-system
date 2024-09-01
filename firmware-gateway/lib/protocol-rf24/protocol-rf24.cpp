#include "./protocol-rf24.hpp"

void sendSignal(RF24 *radio, uint8_t channel)
{
  uint64_t addressSignal = 0xf0f0f0f0f0ULL;

  radio->stopListening();
  radio->setChannel(channel);
  radio->openWritingPipe(addressSignal);

  auto signalPackge = generateSignalPackge();

  radio->write(signalPackge.data, signalPackge.size);
  return;
}

uint8_t getGoodChannel(RF24 *radio, uint32_t timeCheck)
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


uint8_t findChannel(RF24 *radio, uint32_t timeout)
{
  uint64_t addressSignal = 0xf0f0f0f0f0ULL;

  radio->openReadingPipe(0, addressSignal);
  radio->startListening();

  uint8_t channel = 0;

  for (int i = 0; i < NUM_CHANNELS; i++)
  {
    radio->setChannel(i);

    if (radio->available(0))
    {
      char text[32] = "";
      radio->read(&text, sizeof(text));
      Serial.println(text);
    }
  }

  return 0;
}
