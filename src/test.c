#include "uart.h"
#include "type.h"
#include "sleep.h"

int main()
{
    sleep(200000);

    setup38400();

    while(1)
    {
        printString("Hello, world!\n");
        sleep(200000);
    }
}
