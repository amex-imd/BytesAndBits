#include <iostream>
#include "bytesbits.h"
#include <vector>

int main()
{
    short val = 316;

    IMD::BIG_ENDIAN::print_bin_bytes(val);
    std::cout << std::endl;
    bool sign = 0;

    for (int i = 0; i < 6; ++i)
    {
        val = IMD::rotate_carry_left(val, 0, sign);
        IMD::BIG_ENDIAN::print_bin_bytes(val);
        std::cout << std::endl;
    }

    IMD::BIG_ENDIAN::print_bin_bytes(IMD::rotate_carry_left(val, 0, sign));
    std::cout << std::endl
              << sign << std::endl;

    std::cin.get();

    return 0;
}