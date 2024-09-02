
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "printf.h"

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "pack ";

void setup()
{
  Serial.begin(115200);
  printf_begin();
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_2MBPS);
  radio.startListening();
  radio.setChannel(90);
  Serial.println("Init");
}

void loop()
{
  if (radio.available())
  {
    uint8_t text[32] = {0};
    radio.read(&text, sizeof(text));
    Serial.print("Recebido >");
    for (int i = 0; i < 32; i++)
      printf("%u ", text[i]);
    Serial.print("<\n");
  }
}