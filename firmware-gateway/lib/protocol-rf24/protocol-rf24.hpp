#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include "../protocol/protocol.hpp"

#define CE_PIN 7
#define CSN_PIN 8

#define ADDRESS_SGNAL 0xf0f0f0f0f0ULL
#define NUM_CHANNELS 128

void sendSignal(RF24 *radio, uint8_t channel);

uint8_t getGoodChannel(RF24 *radio, uint32_t timeCheck = 128);

uint8_t findChannel(RF24 *radio, uint32_t timeout = 200);
