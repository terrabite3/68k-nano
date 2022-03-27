#include "uart.h"
#include "type.h"
#include "sleep.h"
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
        if (f)
        {
            char buffer[100];
            fgets(buffer, 98, f);

            printString(buffer);
            int err = fclose(f);
            if (err)
            {
                printString("Failed to close FILE.TXT\n");
            }
        }
        else
        {
            printString("Failed to open FILE.TXT\n");
        }

    }
}
