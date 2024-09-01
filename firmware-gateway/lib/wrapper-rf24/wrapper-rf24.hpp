#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 7
#define CSN_PIN 8

#define MSG 0
#define ACK 1
#define RTS 2
#define CTS 3
#define TIMEOUT 500 // milisegundos

class WrapperRF24
{
private:
  RF24 radio = RF24(CE_PIN, CSN_PIN);
  const byte address[6] = "00001";

public:
  WrapperRF24()
  {
    if (!radio.begin())
    {
      Serial.println(F("radio hardware is not responding!!"));
      while (1)
        ;
    }
    radio.setPALevel(RF24_PA_MIN);
    radio.stopListening();
  }
  ~WrapperRF24() {}

  void send(const char *address, size_t  ,const char *data, size_t size)
  {
    radio.openWritingPipe(address);
    const char text[] = "Hello World";
    radio.write(&text, sizeof(text));
    Serial.println("Mensagem enviada: Hello World");
    delay(1000);
  }

  void logic()
  {
  }
};

void printPacote(byte *pac, int tamanho)
{
  Serial.print(F("Rcvd "));
  Serial.print(tamanho); // print the size of the payload
  Serial.print(F(" O: "));
  Serial.print(pac[0]); // print the payload's value
  Serial.print(F(" D: "));
  Serial.print(pac[1]); // print the payload's value
  Serial.print(F(" C: "));
  Serial.print(pac[2]); // print the payload's value
  Serial.print(F(" i: "));
  Serial.print(pac[3]); // print the payload's value
  Serial.print(F(" : "));
  for (int i = 4; i < tamanho; i++)
  {
    Serial.print(pac[i]);
  }
  Serial.println(); // print the payload's value
}

bool aguardaMsg(int tipo)
{
  radio.startListening();
  unsigned long tempoInicio = millis();
  while (millis() - tempoInicio < TIMEOUT)
  {
    if (radio.available())
    {                                         // is there a payload? get the pipe number that recieved it
      uint8_t bytes = radio.getPayloadSize(); // get the size of the payload
      radio.read(&payloadRx[0], bytes);       // fetch payload from FIFO
      if (payloadRx[1] == origem && payloadRx[2] == tipo)
      {

        radio.stopListening();
        return true;
      }
    }
    radio.flush_rx();
    delay(10);
  }
  radio.stopListening();
  return false;
}

bool sendPacket(byte *pacote, int tamanho, int destino, int controle)
{
  pacote[0] = origem;
  pacote[1] = destino;
  pacote[2] = controle;
  pacote[3] = indice;
  for (int i = 0; i < tamanho; i++)
  {
    Serial.print(pacote[i]);
  }
  Serial.println();

  while (1)
  {

    radio.startListening();
    delayMicroseconds(70);
    radio.stopListening();
    if (!radio.testCarrier())
    {
      return radio.write(&pacote[0], tamanho);
    }
    else
    {
      Serial.println("Meio Ocupado");
      delayMicroseconds(270);
    }
    radio.flush_rx();
  }
}

void loop()
{

  if (Serial.available())
  {
    // change the role via the serial monitor

    char c = toupper(Serial.read());
    if (c == 'T')
    {
      // Become the TX node
      unsigned long start_timer = micros();                            // start the timer
      bool report = sendPacket(&payload[0], sizeof(payload), 43, RTS); // transmit & save the report
      report = aguardaMsg(CTS);
      if (report)
      {
        sendPacket(&payload[0], sizeof(payload), 43, MSG);
        report = aguardaMsg(ACK);
      }

      unsigned long end_timer = micros(); // end the timer
      if (report)
      {
        Serial.println("Sucesso!");
      }
      else
      {
        Serial.println("FALHA!");
      }
    }
  }

  radio.flush_rx();
  delay(10);
}