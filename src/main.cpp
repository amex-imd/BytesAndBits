#include <iostream>
#include "bytesbits.h"
#include <vector>

int main()
{
    short val = 316;

    IMD::BIG_ENDIAN::print_bin_bytes(val);
    std::cout << std::endl;

    IMD::BIG_ENDIAN::print_bin_bytes(IMD::rotate_left(val, 9));
    std::cout << std::endl;

    std::cin.get();

    return 0;
}