#include "uart.h"
#include "type.h"
#include "sleep.h"
// #include "cf.h"
// #include "mem-ffs.h"
#include "cf_fat/mem-ffs.h"

#define LED_BOARD  (*((volatile short*) 0x800000))

int main()
{
    LED_BOARD = 0xFFFF;
    sleep(200000);
    LED_BOARD = 1;

    setup38400();

    while(1)
    {
        printString("Hello, world!\n");
        sleep(200000);
        // LED_BOARD = 2;
        ffs_process();

        if (ffs_is_card_available())
        {
            printString("Card available\n");
        }
        else
        {
            printString("Card not available\n");
        }

        FFS_FILE* f = fopen("FILE.TXT", "r");
        char buffer[100];
        fgets(buffer, 98, f);

        printString(buffer);

        fclose(f);

        // LED_BOARD = 0xFF;
        // LED_BOARD = 0;
        // LED_BOARD = 0xFF;
        // LED_BOARD = 0;
        // LED_BOARD = 0xFF;
        // LED_BOARD = 0;
        // LED_BOARD = 0xFF;
        // LED_BOARD = 0;
        // LED_BOARD = 0xFF;
        // LED_BOARD = 0;
        // LED_BOARD = 0xFF;
        // LED_BOARD = 0;
        // LED_BOARD = 0xFF;
        // LED_BOARD = 0;
        LED_BOARD = 0xFF;
        // LED_BOARD = 0;
        ffs_process();

        // printString("\nReading CompactFlash ID\n");
        // uint16_t buffer[SECTOR_BUFFER_LEN];
        // cf_identify(buffer, SECTOR_BUFFER_LEN);
        // printHexBuffer(buffer, SECTOR_BUFFER_LEN);
        // printString("\n");
        // sleep(200000);


        // printString("\nReading Sector 1\n");
        // cf_read_sector(1, buffer, SECTOR_BUFFER_LEN);
        // printHexBuffer(buffer, SECTOR_BUFFER_LEN);
        // printString("\n");
        // sleep(200000);
    }
}
