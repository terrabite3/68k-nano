#include "uart.h"
#include "type.h"
#include "sleep.h"
#include "cf.h"

int main()
{
    sleep(200000);

    setup38400();

    while(1)
    {
        printString("Hello, world!\n");
        sleep(200000);

        printString("\nReading CompactFlash ID\n");
        uint16_t buffer[SECTOR_BUFFER_LEN];
        cf_identify(buffer, SECTOR_BUFFER_LEN);
        printHexBuffer(buffer, SECTOR_BUFFER_LEN);
        printString("\n");
        sleep(200000);
    }
}
