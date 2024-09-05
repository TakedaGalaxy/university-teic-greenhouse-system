#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include "../protocol/protocol.hpp"

#define CE_PIN 7
#define CSN_PIN 8

#define ADDRESS_PACKGE 0xf0f0f0f0f0ULL
#define NUM_CHANNELS 52
#define PROTOCOL_CHANNEL 52

void sendSignal(RF24 *radio, uint8_t channel);

uint8_t getGoodChannel(RF24 *radio, uint32_t timeCheck = 128);

uint8_t findChannel(RF24 *radio, uint32_t timeout = 200);

RF24 *configRF24Sensor(uint8_t ce, uint8_t csn);

bool isPackAvailable(RF24 *radio);

PackgeSerialized readPackge(RF24 *radio);

void sendPackge(RF24 *radio, PackgeSerialized packge);

RF24 *configRF24Transmiter(uint8_t ce, uint8_t csn);

bool isFreeToSendPackge(RF24 *radio);

void sendPackge(RF24 *radio, PackgeSerialized packge);