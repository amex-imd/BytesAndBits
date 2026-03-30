#include <iostream>
#include "bytesbits.h"
#include <vector>

int main()
{
    short val = 314;
    IMD::BIG_ENDIAN::print_bits(val);
    int j = 0;
    while (j < 16)
    {
        std::cout << IMD::BIG_ENDIAN::get_bit(val, j);
        ++j;
    }

    auto c = IMD::BIG_ENDIAN::to_bit_vector(val);
    for (auto x : c)
        std::cout << x << " ";
    auto data = IMD::BIG_ENDIAN::restore_value_from_bits<short>(c.begin(), c.end());
    std::cout << data;

    std::cout << std::endl;

    auto v = IMD::ECC::triple_repeat_encode(val);
    int i(0);
    for (auto x : v)
    {

        std::cout << x << " ";
        ++i;
        if (i % 3 == 0)
            std::cout << std::endl;
    }
    v[1] = 1;
    std::cout << "------------" << std::endl;
    for (auto x : v)
    {

        std::cout << x << " ";
        ++i;
        if (i % 3 == 0)
            std::cout << std::endl;
    }
    std::cout << "------------" << std::endl;
    auto res = IMD::ECC::triple_repeat_decode<short>(v);
    std::cout << "error_corrected: " << res.error_corrected << " error_detected: " << res.error_detected << " errors_number: " << res.errors_number << " restored_value: " << res.data;

    std::cin.get();
    return 0;
}