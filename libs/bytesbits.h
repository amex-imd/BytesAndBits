#ifndef __BYTESBITS_IMD_
#define __BYTESBITS_IMD_

#include <iostream>
#include <iomanip>
#include <algorithm>

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

    namespace BIG_ENDIAN
    {
        // PRINT METHODS

        template <typename T>
        void print_bits(const T &val, const char *sep = " ", std::ostream &os = std::cout)
        {
            auto ptr = reinterpret_cast<const std::byte *>(&val);
            auto bytes = bytes_number<T>();

            for (size_t i(bytes); i-- > 0;)
            {
                for (size_t j(BITS_PER_BYTE); j-- > 0;)
                    os << (static_cast<int>(ptr[i] >> j) & 1);
                os << sep;
            }
        }

        template <typename T>
        void print_bin_bytes(const T &val, const char *sep = " ", std::ostream &os = std::cout)
        {
            auto ptr = reinterpret_cast<const std::byte *>(&val);
            auto bytes = bytes_number<T>();

            for (size_t i(bytes); i-- > 0;)
            {
                os << "0b";
                for (size_t j(BITS_PER_BYTE); j-- > 0;)
                    os << (static_cast<int>(ptr[i] >> j) & 1);
                if (i != 0)
                    os << sep;
            }
        }

        template <typename T>
        void print_oct_bytes(const T &val, const char *sep = " ", std::ostream &os = std::cout)
        {
            auto ptr = reinterpret_cast<const std::byte *>(&val);
            auto bytes = bytes_number<T>();

            for (size_t i(bytes); i-- > 0;)
            {
                os << "0o" << std::setw(3) << std::setfill('0') << std::oct << static_cast<int>(ptr[i]);
                if (i != 0)
                    os << sep;
            }
            os << std::dec;
        }

        template <typename T>
        void print_dec_bytes(const T &val, const char *sep = " ", std::ostream &os = std::cout)
        {
            auto ptr = reinterpret_cast<const std::byte *>(&val);
            auto bytes = bytes_number<T>();

            for (size_t i(bytes); i-- > 0;)
            {
                os << std::dec << static_cast<int>(ptr[i]);
                if (i != 0)
                    os << sep;
            }
        }

        template <typename T>
        void print_hex_bytes(const T &val, const char *sep = " ", std::ostream &os = std::cout)
        {
            auto ptr = reinterpret_cast<const std::byte *>(&val);
            auto bytes = bytes_number<T>();

            for (size_t i(bytes); i-- > 0;)
            {
                os << "0x" << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(ptr[i]);
                if (i != 0)
                    os << sep;
            }
            os << std::dec;
        }

        // MODIFY METHODS

        template <typename T>
        void modify_bit(T &val, size_t bit_ind, bool bit_val)
        {
            if (bit_ind >= bits_number<T>())
                throw std::out_of_range("The bit index is out of range");

            auto ptr = reinterpret_cast<std::byte *>(&val);

            size_t byte_ind = bytes_number<T>() - 1 - bit_ind / BITS_PER_BYTE;
            size_t bit_ind_in_byte = BITS_PER_BYTE - 1 - bit_ind % BITS_PER_BYTE;

            unsigned char byte_val = static_cast<unsigned char>(ptr[byte_ind]);

            if (bit_val)
                byte_val |= (1 << bit_ind_in_byte);
            else
                byte_val &= ~(1 << bit_ind_in_byte);

            ptr[byte_ind] = static_cast<std::byte>(byte_val);
        }
        template <typename T>
        void set_bit(T &val, size_t bit_ind)
        {
            modify_bit(val, bit_ind, 1);
        }

        template <typename T>
        void reset_bit(T &val, size_t bit_ind)
        {
            modify_bit(val, bit_ind, 0);
        }
        template <typename T>
        bool get_bit(const T &val, size_t bit_ind)
        {
            if (bit_ind >= bits_number<T>())
                throw std::out_of_range("The bit index is out of range");

            auto ptr = reinterpret_cast<const std::byte *>(&val);
            size_t byte_ind = bytes_number<T>() - 1 - (bit_ind / BITS_PER_BYTE);
            size_t bit_ind_in_byte = bit_ind % BITS_PER_BYTE;

            return (static_cast<int>(ptr[byte_ind]) >> bit_ind_in_byte) & 1;
        }

        // RESTORE METHODS

        template <typename T, typename InputIt>
        T restore_value_from_bytes(InputIt beg, InputIt end)
        {
            if (std::distance(beg, end) != bytes_number<T>())
                throw std::runtime_error("The number of elements in the range does not match the size of the type T");

            T res;
            auto ptr = reinterpret_cast<std::byte *>(&res);
            size_t ind(bytes_number<T>() - 1);

            while (beg != end)
            {
                ptr[ind] = static_cast<std::byte>(*beg);
                --ind;
                ++beg;
            }

            return res;
        }
        template <typename T, typename InputIt>
        T restore_value_from_bits(InputIt beg, InputIt end)
        {
            if (std::distance(beg, end) != bits_number<T>())
                throw std::runtime_error("The number of bits does not match the size of type T");

            T res = 0;
            size_t bit_ind = 0;
            size_t total_bits = bits_number<T>();

            while (beg != end)
            {
                bool bit_val = static_cast<bool>(*beg);

                if (bit_val)
                    res |= (static_cast<T>(1) << (total_bits - 1 - bit_ind));

                ++bit_ind;
                ++beg;
            }

            return res;
        }
    }

    namespace LITTLE_ENDIAN
    {
        // PRINT METHODS

        template <typename T>
        void print_bits(const T &val, const char *sep = " ", std::ostream &os = std::cout)
        {
            auto ptr = reinterpret_cast<const std::byte *>(&val);
            auto bytes = bytes_number<T>();

            for (size_t i(0); i < bytes; ++i)
            {
                for (size_t j(BITS_PER_BYTE); j-- > 0;)
                    os << (static_cast<int>(ptr[i] >> j) & 1);
                os << sep;
            }
        }

        template <typename T>
        void print_bin_bytes(const T &val, const char *sep = " ", std::ostream &os = std::cout)
        {
            auto ptr = reinterpret_cast<const std::byte *>(&val);
            auto bytes = bytes_number<T>();

            for (size_t i(0); i < bytes; ++i)
            {
                os << "0b";
                for (size_t j(BITS_PER_BYTE); j-- > 0;)
                    os << (static_cast<int>(ptr[i] >> j) & 1);
                if (i != bytes - 1)
                    os << sep;
            }
        }

        template <typename T>
        void print_oct_bytes(const T &val, const char *sep = " ", std::ostream &os = std::cout)
        {
            auto ptr = reinterpret_cast<const std::byte *>(&val);
            auto bytes = bytes_number<T>();

            for (size_t i(0); i < bytes; ++i)
            {
                os << "0o" << std::setw(3) << std::setfill('0') << std::oct << static_cast<int>(ptr[i]);
                if (i != bytes - 1)
                    os << sep;
            }
            os << std::dec;
        }

        template <typename T>
        void print_dec_bytes(const T &val, const char *sep = " ", std::ostream &os = std::cout)
        {
            auto ptr = reinterpret_cast<const std::byte *>(&val);
            auto bytes = bytes_number<T>();

            for (size_t i(0); i < bytes; ++i)
            {
                os << std::dec << static_cast<int>(ptr[i]);
                if (i != bytes - 1)
                    os << sep;
            }
        }

        template <typename T>
        void print_hex_bytes(const T &val, const char *sep = " ", std::ostream &os = std::cout)
        {
            auto ptr = reinterpret_cast<const std::byte *>(&val);
            auto bytes = bytes_number<T>();

            for (size_t i(0); i < bytes; ++i)
            {
                os << "0x" << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(ptr[i]);
                if (i != bytes - 1)
                    os << sep;
            }
            os << std::dec;
        }

        // MODIFY METHODS

        template <typename T>
        void modify_bit(T &val, size_t bit_ind, bool bit_val)
        {
            if (bit_ind >= bits_number<T>())
                throw std::out_of_range("The bit index is out of range");

            auto ptr = reinterpret_cast<std::byte *>(&val);

            size_t byte_ind = bit_ind / BITS_PER_BYTE;
            size_t bit_ind_in_byte = BITS_PER_BYTE - 1 - bit_ind % BITS_PER_BYTE;

            unsigned char byte_val = static_cast<unsigned char>(ptr[byte_ind]);

            if (bit_val)
                byte_val |= (1 << bit_ind_in_byte);
            else
                byte_val &= ~(1 << bit_ind_in_byte);

            ptr[byte_ind] = static_cast<std::byte>(byte_val);
        }
        template <typename T>
        void set_bit(T &val, size_t bit_ind)
        {
            modify_bit(val, bit_ind, 1);
        }

        template <typename T>
        void reset_bit(T &val, size_t bit_ind)
        {
            modify_bit(val, bit_ind, 0);
        }
        template <typename T>
        bool get_bit(const T &val, size_t bit_ind)
        {
            if (bit_ind >= bits_number<T>())
                throw std::out_of_range("The bit index is out of range");

            auto ptr = reinterpret_cast<const std::byte *>(&val);
            size_t byte_ind = bit_ind / BITS_PER_BYTE;
            size_t bit_ind_in_byte = bit_ind % BITS_PER_BYTE;

            return (static_cast<int>(ptr[byte_ind]) >> bit_ind_in_byte) & 1;
        }

        // RESTORING METHODS

        template <typename T, typename InputIt>
        T restore_value_from_bytes(InputIt beg, InputIt end)
        {
            if (std::distance(beg, end) != bytes_number<T>())
                throw std::runtime_error("The number of elements in the range does not match the size of the type T");

            T res;
            auto ptr = reinterpret_cast<std::byte *>(&res);
            size_t ind(0);

            while (beg != end)
            {
                ptr[ind] = static_cast<std::byte>(*beg);
                ++ind;
                ++beg;
            }

            return res;
        }
        template <typename T, typename InputIt>
        T restore_value_from_bits(InputIt beg, InputIt end)
        {
            if (std::distance(beg, end) != bits_number<T>())
                throw std::runtime_error("The number of bits does not match the size of type T");

            T res = 0;
            size_t bit_ind = 0;

            while (beg != end)
            {
                bool bit_val = static_cast<bool>(*beg);

                if (bit_val)
                {
                    size_t byte = bit_ind / BITS_PER_BYTE;
                    size_t bit = BITS_PER_BYTE - 1 - bit_ind % BITS_PER_BYTE;

                    res |= (static_cast<T>(1) << (byte * BITS_PER_BYTE + bit));
                }

                ++bit_ind;
                ++beg;
            }

            return res;
        }
    }

    // METHODS OF BITS NUMBER

    template <typename T>
    bool all_bits_one(const T &val)
    {
        auto ptr = reinterpret_cast<const std::byte *>(&val);
        auto max = std::byte((1 << BITS_PER_BYTE) - 1);
        size_t L = bytes_number<T>();

        for (size_t i(0); i < L; ++i)
            if (ptr[i] != max)
                return false;
        return true;
    }
    template <typename T>
    bool all_bits_zero(const T &val)
    {
        auto ptr = reinterpret_cast<const std::byte *>(&val);
        size_t bytes = bytes_number<T>();

        for (size_t i(0); i < bytes; ++i)
            if (ptr[i] != std::byte(0))
                return false;
        return true;
    }
    template <typename T>
    bool any_bits_zero(const T &val)
    {
        return !all_bits_one(val);
    }

    template <typename T>
    bool any_bits_one(const T &val)
    {
        return !all_bits_zero(val);
    }

    template <typename T>
    void fill_one_bit(T &val)
    {
        auto ptr = reinterpret_cast<std::byte *>(&val);
        size_t bytes = bytes_number<T>();
        std::byte max = std::byte((1 << BITS_PER_BYTE) - 1);

        std::fill(ptr, ptr + bytes, max);
    }

    template <typename T>
    void fill_zero_bit(T &val)
    {
        auto ptr = reinterpret_cast<std::byte *>(&val);
        size_t bytes = bytes_number<T>();

        std::fill(ptr, ptr + bytes, std::byte(0));
    }

    template <typename T>
    size_t zero_bit_number(const T &val)
    {
        auto ptr = reinterpret_cast<const std::byte *>(&val);
        size_t res(0);
        size_t bytes = bytes_number<T>();

        for (size_t i(bytes); i-- > 0;)
            for (size_t j(BITS_PER_BYTE); j-- > 0;)
                if (((static_cast<unsigned char>(ptr[i]) >> j) & 1) == 0)
                    ++res;

        return res;
    }

    template <typename T>
    size_t one_bit_number(const T &val)
    {
        return bits_number<T>() - zero_bit_number(val);
    }

    // REVERSING METHODS

    template <typename T>
    void reverse_bytes(T &val)
    {
        auto ptr = reinterpret_cast<std::byte *>(&val);
        size_t bytes = bytes_number<T>();
        for (size_t i(0); i < bytes / 2; ++i)
            std::swap(ptr[i], ptr[bytes - 1 - i]);
    }
    template <typename T>
    void reverse_bits(T &val)
    {
        auto ptr = reinterpret_cast<std::byte *>(&val);
        size_t bytes = bytes_number<T>();

        for (size_t i(0); i < bytes; ++i)
        {
            unsigned char byte = static_cast<unsigned char>(ptr[i]);
            unsigned char reversed_byte = 0;
            for (size_t j(0); j < BITS_PER_BYTE; ++j)
                reversed_byte |= ((byte >> j) & 1) << (BITS_PER_BYTE - 1 - j);
            ptr[i] = static_cast<std::byte>(reversed_byte);
        }

        reverse_bytes(val);
    }

    // ROTATING METHODS

    template <typename T>
    constexpr T rotate_left(const T &val, size_t shift)
    {
        if constexpr (std::is_signed_v<T>)
            if (val < 0)
                throw std::invalid_argument("The val must be non-negative for signed types");

        size_t bits = bits_number<T>();
        shift %= bits;

        if (shift == 0)
            return val;

        return (val << shift) | (val >> (bits - shift));
    }

    template <typename T>
    constexpr T rotate_right(const T &val, size_t shift)
    {
        if constexpr (std::is_signed_v<T>)
            if (val < 0)
                throw std::invalid_argument("The val must be non-negative for signed types");

        size_t bits = bits_number<T>();
        shift %= bits;

        if (shift == 0)
            return val;

        return (val >> shift) | (val << (bits - shift));
    }
    template <typename T>
    constexpr T rotate_carry_left(const T &val, bool carry_in, bool &carry_out)
    {
        if constexpr (std::is_signed_v<T>)
            if (val < 0)
                throw std::invalid_argument("The val must be non-negative for signed types");

        constexpr size_t bits = bits_number<T>();

        carry_out = (val >> (bits - 1)) & 1;

        return (val << 1) | (carry_in ? 1 : 0);
    }

    template <typename T>
    constexpr T rotate_carry_right(const T &val, bool carry_in, bool &carry_out)
    {
        if constexpr (std::is_signed_v<T>)
            if (val < 0)
                throw std::invalid_argument("The val must be non-negative for signed types");

        constexpr size_t bits = bits_number<T>();

        carry_out = val & 1;

        return (val >> 1) | (carry_in ? (1 << (bits - 1)) : 0);
    }
}

#endif