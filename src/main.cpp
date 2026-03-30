#include <iostream>
#include "bytesbits.h"
#include <vector>

int main()
{
    short val = 314;
    IMD::BIG_ENDIAN::print_bits(val);

    auto v = IMD::ECC::add_parity_bit(val);
    for (auto x : v)
        std::cout << x << " ";
    std::cout << std::endl;
    size_t size = v.size();
    v[size - 1] = 0;

    std::cout << IMD::ECC::check_parity_bit(v);

    std::cin.get();

    return 0;
}