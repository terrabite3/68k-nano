#include "uart.h"

#include "type.h"


// The UART is connected to the upper half of the data bus, 
// which means it is accessed at even addresses
volatile char * const uart     = (char *) 0xA00000;
volatile char * const uart_RBR = (char *) 0xA00000; // Receiver Buffer Register (Read)
volatile char * const uart_THR = (char *) 0xA00000; // Transmitter Holding Register (Write)
volatile char * const uart_DLL = (char *) 0xA00000; // Divisor LSB (set DLAB first)
volatile char * const uart_IER = (char *) 0xA00002; // Interrupt Enable Register
volatile char * const uart_DLM = (char *) 0xA00002; // Divisor MSB (set DLAB first)
volatile char * const uart_IIR = (char *) 0xA00004; // Interrupt Ident. Register (Read)
volatile char * const uart_FCR = (char *) 0xA00004; // FIFO Control Register (Write)
volatile char * const uart_LCR = (char *) 0xA00006; // Line Control Register
volatile char * const uart_MCR = (char *) 0xA00008; // Modem Control Register
volatile char * const uart_LSR = (char *) 0xA0000A; // Line Status Register
volatile char * const uart_MSR = (char *) 0xA0000C; // Modem Status Register
volatile char * const uart_SCR = (char *) 0xA0000E; // Scratch Register


void setup38400()
{
    // 8 data bits, 1 stop bit, no parity, DLAB
    *uart_LCR = 0b10000011;
    // the ROM writes 0x09 to DLL, 0x00 to DLM
    // We'll set DLL to 13, DLM to 0 for a much better approximation of 38400
    // than we can get to 57600 with an 8MHz crystal
    *uart_DLL = 13;
    *uart_DLM = 0;
    *uart_LCR = 0b00000011;
}

void printString(const char* s)
{
    while (*s)
    {
        // Check THRE - transmitter holding register empty
        if (*uart_LSR & 0x20)
        {
            *uart = *s;
            s++;
        }
    }
}

void printHex16(uint16_t num)
{
    char digits[7];
    digits[0] = '0';
    digits[1] = 'x';
    digits[6] = 0;
    for (uint8_t i = 0; i < 4; ++i)
    {
        uint8_t x = (num >> (12 - 4 * i)) & 0xF;
        if (x < 10) 
            digits[i + 2] = x + '0';
        else
            digits[i + 2] = x + 'A' - 10;
    }
    printString(digits);
}

void printHex16No0x(uint16_t num)
{
    char digits[5];
    digits[4] = 0;
    for (uint8_t i = 0; i < 4; ++i)
    {
        uint8_t x = (num >> (12 - 4 * i)) & 0xF;
        if (x < 10) 
            digits[i] = x + '0';
        else
            digits[i] = x + 'a' - 10;
    }
    printString(digits);
}

char printableOrDot(char c)
{
    if (c >= ' ' && c <= '~')
        return c;
    else
        return '.';
}

void printHexBuffer(uint16_t* buffer, uint16_t buffer_len)
{
    uint16_t completeRows = buffer_len / 8;
    uint16_t wordsOnIncompleteRow = buffer_len % 8;

    for (uint16_t row = 0; row < completeRows; ++row)
    {
        uint16_t offset = row << 3;
        printHex16No0x(row << 4);
        printString(": ");

        char text[17];
        text[16] = 0;

        for (uint8_t col = 0; col < 8; ++col)
        {
            printHex16No0x(buffer[col + offset]);
            printString(" ");

            text[2 * col + 1] = printableOrDot(buffer[col + offset] & 0xFF);
            text[2 * col] = printableOrDot((buffer[col + offset] >> 8) & 0xFF);
        }

        printString(" ");
        printString(text);
        printString("\n");
    }
}
