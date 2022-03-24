#pragma once

#include "type.h"

// Setup for 38400 baud, which is a much better approximation with an 8 MHz crystal
// 8 data bits, 1 stop bit, no parity
void setup38400();

void printString(const char* s);

void printHex16(uint16_t num);

// Print a hex editor style output
// buffer_len is the number of WORDS, not BYTES
void printHexBuffer(uint16_t* buffer, uint16_t buffer_len);

