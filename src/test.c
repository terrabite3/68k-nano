volatile short * const ledBoard = (short *)0x800000;

int main()
{
    short counter = 0;
    while(1)
    {
        *ledBoard = counter;
        counter++;
    }
}
