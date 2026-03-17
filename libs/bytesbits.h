#ifndef __BYTESBITS_IMD_
#define __BYTESBITS_IMD_

#include <iostream>

namespace IMD
{
    constexpr size_t BITS_PER_BYTE(8);

    template <typename T>
    constexpr size_t bytes_number() noexcept
    {
        return sizeof(T);
    }

    template <typename T>
    constexpr size_t bits_number() noexcept
    {
        return bytes_number<T>() * BITS_PER_BYTE;
    }

    template <typename T>
    void print_info(std::ostream &os = std::cout)
    {
        os << "Type name: " << typeid(T).name() << std::endl
           << "Number of bytes: " << bytes_number<T>() << std::endl
           << "Number of bits: " << bits_number<T>();
    }
    template <typename T>
    void println_info(std::ostream &os = std::cout)
    {
        print_info<T>(os);
        os << std::endl;
    }

}

#endif