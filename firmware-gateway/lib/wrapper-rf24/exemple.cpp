#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Pinos do CE e CSN
#define CE_PIN   9
#define CSN_PIN  10

// Cria um objeto RF24
RF24 radio(CE_PIN, CSN_PIN);

// Endereço de transmissão
const byte address[6] = "00001";

void setup() {
  Serial.begin(115200);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void loop() {
  const char text[] = "Hello World";
  radio.write(&text, sizeof(text));
  Serial.println("Mensagem enviada: Hello World");
  delay(1000);
}
///////////////

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Pinos do CE e CSN
#define CE_PIN   9
#define CSN_PIN  10

// Cria um objeto RF24
RF24 radio(CE_PIN, CSN_PIN);

// Endereço de recepção
const byte address[6] = "00001";

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.println("Mensagem recebida: ");
    Serial.println(text);
  }
}
