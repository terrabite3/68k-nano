#include "cf.h"



volatile short * const CF_DATA =        (unsigned short *)   0x900000;   // RW
// volatile char * const cfCard_error =        (char *)    0x900003;   // R
// volatile char * const cfCard_feature =      (char *)    0x900003;   // W
// volatile char * const cfCard_count =        (char *)    0x900005;   // RW
// volatile char * const cfCard_sector =       (char *)    0x900007;   // RW
// volatile char * const cfCard_lba0 =         (char *)    0x900007;   // RW
// volatile char * const cfCard_cyl_low =      (char *)    0x900009;   // RW
// volatile char * const cfCard_lba1 =         (char *)    0x900009;   // RW
// volatile char * const cfCard_cyl_high =     (char *)    0x90000B;   // RW
// volatile char * const cfCard_lba2 =         (char *)    0x90000B;   // RW
// volatile char * const cfCard_head =         (char *)    0x90000D;   // RW
// volatile char * const cfCard_lba3 =         (char *)    0x90000D;   // RW
volatile char * const CF_STATUS =       (unsigned char *)    0x90000F;   // R
volatile char * const CF_COMMAND =      (unsigned char *)    0x90000F;   // W


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
    *CF_COMMAND = 0xEC;

    // Waiting for DRQ
    while(!((*CF_STATUS) & 0x08)); 

    return readSector(buffer, buffer_len);
}
