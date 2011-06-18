#include <iostream>

int main(void)
{
    int count = 0;
    while (! std::cin.eof())
    {
        char ch;
        std::cin >> ch;
        switch(count++ & 0x3)
        {
            case 0: ch ^= 0x49; break;
            case 1: ch ^= 0x43; break;
            case 2: ch ^= 0x46; break;
            case 3: ch ^= 0x50; break;
        }
        std::cout << ch;
    }

    return 0;
}
