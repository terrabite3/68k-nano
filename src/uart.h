#pragma once

// Setup for 38400 baud, which is a much better approximation with an 8 MHz crystal
// 8 data bits, 1 stop bit, no parity
void setup38400();

void printString(const char* s);


