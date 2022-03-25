#include "cf.h"



volatile uint16_t * const CF_DATA =        (uint16_t *)   0x900000;   // RW
volatile uint8_t * const CF_ERROR =        (uint8_t *)    0x900003;   // R
volatile uint8_t * const CF_FEATURE =      (uint8_t *)    0x900003;   // W
volatile uint8_t * const CF_COUNT =        (uint8_t *)    0x900005;   // RW
volatile uint8_t * const CF_LBA0 =         (uint8_t *)    0x900007;   // RW
volatile uint8_t * const CF_LBA1 =         (uint8_t *)    0x900009;   // RW
volatile uint8_t * const CF_LBA2 =         (uint8_t *)    0x90000B;   // RW
volatile uint8_t * const CF_LBA3 =         (uint8_t *)    0x90000D;   // RW
volatile uint8_t * const CF_STATUS =       (uint8_t *)    0x90000F;   // R
volatile uint8_t * const CF_COMMAND =      (uint8_t *)    0x90000F;   // W

const static uint8_t CF_COMMAND_READ_SECTOR = 0x20;
const static uint8_t CF_COMMAND_ID = 0xEC;

uint16_t readSector(uint16_t* buffer, uint16_t buffer_len)
{
    uint16_t wordsRead = 0;

    for (uint16_t i = 0; i < 256; ++i)
    {
        uint16_t d = *CF_DATA;

        if (i < buffer_len)
        {
            buffer[i] = d;
            wordsRead++;
        }

        // Do we actually need to finish reading the sector?
    }

    return wordsRead;
}


uint16_t cf_identify(uint16_t* buffer, uint16_t buffer_len)
{
    // Waiting for drive not busy
    while((*CF_STATUS) & 0x80);

    // Sending Identify command
    *CF_COMMAND = CF_COMMAND_ID;

    // Waiting for DRQ
    while(!((*CF_STATUS) & 0x08)); 

    return readSector(buffer, buffer_len);
}

void byteSwap(uint16_t* buffer, uint16_t buffer_len)
{
    for (uint16_t i = 0; i < buffer_len; ++i)
    {
        // uint8_t a = buffer[i] & 0xFF;
        // uint8_t b = (buffer[i] >> 8) & 0xFF;
        // buffer[i] = a << 8 | b;
        buffer[i] = buffer[i] >> 8 | buffer[i] << 8;
    }
}

uint16_t cf_read_sector(uint32_t sector, uint16_t* buffer, uint16_t buffer_len)
{
    // Set sector count (1 for now)
    *CF_COUNT = 1;

    // Set LBA
    *CF_LBA0 =  sector        & 0xFF;
    *CF_LBA1 = (sector >> 8)  & 0xFF;
    *CF_LBA2 = (sector >> 16) & 0xFF;
    *CF_LBA3 = (sector >> 24) & 0xFF;

    // Waiting for drive not busy
    while((*CF_STATUS) & 0x80);

    // Sending Read Sectors command
    *CF_COMMAND = CF_COMMAND_READ_SECTOR;

    // Waiting for DRQ
    while(!((*CF_STATUS) & 0x08)); 

    readSector(buffer, buffer_len);
    byteSwap(buffer, buffer_len);
}
