#ifndef __BYTESBITS_IMD_
#define __BYTESBITS_IMD_

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <vector>
#include <tuple>
#include <string>
#include <cstddef>
#include <typeinfo>

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
    namespace BIG_ENDIAN
    {
        // --------Print methods--------

        template <typename T>
        void print_bits(const T &val, const char *sep = " ", std::ostream &os = std::cout)
        {
            auto ptr = reinterpret_cast<const std::byte *>(&val);
            auto B(bytes_number<T>());

            for (size_t i(B); i-- > 0;)
            {
                for (size_t j(BITS_PER_BYTE); j-- > 0;)
                    os << (static_cast<int>(ptr[i] >> j) & 1);
                os << sep;
            }
        }
        template <typename T>
        void println_bits(const T &val, const char *sep = " ", std::ostream &os = std::cout)
        {
            print_bits(val, sep, os);
            os << std::endl;
        }

        template <typename T>
        void print_bin_bytes(const T &val, const char *sep = " ", std::ostream &os = std::cout)
        {
            auto ptr = reinterpret_cast<const std::byte *>(&val);
            auto B(bytes_number<T>());

            for (size_t i(B); i-- > 0;)
            {
                os << "0b";
                for (size_t j(BITS_PER_BYTE); j-- > 0;)
                    os << (static_cast<int>(ptr[i] >> j) & 1);
                if (i != 0)
                    os << sep;
            }
        }
        template <typename T>
        void println_bin_bytes(const T &val, const char *sep = " ", std::ostream &os = std::cout)
        {
            print_bin_bytes(val, sep, os);
            os << std::endl;
        }

        template <typename T>
        void print_oct_bytes(const T &val, const char *sep = " ", std::ostream &os = std::cout)
        {
            auto ptr = reinterpret_cast<const std::byte *>(&val);
            auto B(bytes_number<T>());

            for (size_t i(B); i-- > 0;)
            {
                os << "0o" << std::setw(3) << std::setfill('0') << std::oct << static_cast<int>(ptr[i]);
                if (i != 0)
                    os << sep;
            }
            os << std::dec;
        }
        template <typename T>
        void println_oct_bytes(const T &val, const char *sep = " ", std::ostream &os = std::cout)
        {
            print_oct_bytes(val, sep, os);
            os << std::endl;
        }

        template <typename T>
        void print_dec_bytes(const T &val, const char *sep = " ", std::ostream &os = std::cout)
        {
            auto ptr = reinterpret_cast<const std::byte *>(&val);
            auto B(bytes_number<T>());

            for (size_t i(B); i-- > 0;)
            {
                os << std::dec << static_cast<int>(ptr[i]);
                if (i != 0)
                    os << sep;
            }
        }
        template <typename T>
        void println_dec_bytes(const T &data, const char *sep = " ", std::ostream &os = std::cout)
        {
            print_dec_bytes(data, sep, os);
            os << std::endl;
        }

        template <typename T>
        void print_hex_bytes(const T &val, const char *sep = " ", std::ostream &os = std::cout)
        {
            auto ptr = reinterpret_cast<const std::byte *>(&val);
            auto B(bytes_number<T>());

            for (size_t i(B); i-- > 0;)
            {
                os << "0x" << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(ptr[i]);
                if (i != 0)
                    os << sep;
            }
            os << std::dec;
        }
        template <typename T>
        void println_hex_bytes(const T &val, const char *sep = " ", std::ostream &os = std::cout)
        {
            print_hex_bytes(val, sep, os);
            os << std::endl;
        }

        // --------Modify methods--------

        template <typename T>
        void modify_bit(T &val, size_t bit_ind, bool bit_val)
        {
            if (bit_ind >= bits_number<T>())
                throw std::out_of_range("The bit index is out of range");

            auto ptr = reinterpret_cast<std::byte *>(&val);

            // Calculating the real position
            size_t byte_ind(bytes_number<T>() - 1 - bit_ind / BITS_PER_BYTE);
            size_t bit_ind_in_byte(BITS_PER_BYTE - 1 - bit_ind % BITS_PER_BYTE);

            unsigned char tmp = static_cast<unsigned char>(ptr[byte_ind]);

            if (bit_val)
                tmp |= (1 << bit_ind_in_byte);
            else
                tmp &= ~(1 << bit_ind_in_byte);

            ptr[byte_ind] = static_cast<std::byte>(tmp);
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
            size_t bit_ind_in_byte(BITS_PER_BYTE - 1 - bit_ind % BITS_PER_BYTE);

            return (static_cast<int>(ptr[byte_ind]) >> bit_ind_in_byte) & 1;
        }

        // --------Restore methods--------

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
            size_t bit_ind(0);
            size_t total_bits(bits_number<T>());

            while (beg != end)
            {
                bool tmp = static_cast<bool>(*beg);

                if (tmp)
                    res |= (static_cast<T>(1) << (total_bits - 1 - bit_ind));

                ++bit_ind;
                ++beg;
            }

            return res;
        }

        template <typename T>
        std::vector<std::byte> to_byte_vector(const T &data)
        {
            std::vector<std::byte> res;
            size_t B(bytes_number<T>());
            res.reserve(B);
            auto ptr = reinterpret_cast<const std::byte *>(&data);

            for (size_t i(B); i-- > 0;)
                res.push_back(ptr[i]);

            return res;
        }
        template <typename T>
        std::vector<bool> to_bit_vector(const T &data)
        {
            std::vector<bool> res;
            size_t B(bytes_number<T>());
            res.reserve(bits_number<T>());
            auto ptr = reinterpret_cast<const std::byte *>(&data);

            for (size_t i(B); i-- > 0;)
            {
                unsigned char byte = static_cast<unsigned char>(ptr[i]);
                for (size_t j = BITS_PER_BYTE; j-- > 0;)
                    res.push_back((byte >> j) & 1);
            }

            return res;
        }
    }

    namespace LITTLE_ENDIAN
    {
        // --------Print methods--------

        template <typename T>
        void print_bits(const T &val, const char *sep = " ", std::ostream &os = std::cout)
        {
            auto ptr = reinterpret_cast<const std::byte *>(&val);
            auto B(bytes_number<T>());

            for (size_t i(0); i < B; ++i)
            {
                for (size_t j(BITS_PER_BYTE); j-- > 0;)
                    os << (static_cast<int>(ptr[i] >> j) & 1);
                os << sep;
            }
        }
        template <typename T>
        void println_bits(const T &val, const char *sep = " ", std::ostream &os = std::cout)
        {
            print_bits(val, sep, os);
            os << std::endl;
        }

        template <typename T>
        void print_bin_bytes(const T &val, const char *sep = " ", std::ostream &os = std::cout)
        {
            auto ptr = reinterpret_cast<const std::byte *>(&val);
            auto B(bytes_number<T>());

            for (size_t i(0); i < B; ++i)
            {
                os << "0b";
                for (size_t j(BITS_PER_BYTE); j-- > 0;)
                    os << (static_cast<int>(ptr[i] >> j) & 1);
                if (i != B - 1)
                    os << sep;
            }
        }

        template <typename T>
        void println_bin_bytes(const T &val, const char *sep = " ", std::ostream &os = std::cout)
        {
            print_bin_bytes(val, sep, os);
            os << std::endl;
        }

        template <typename T>
        void print_oct_bytes(const T &val, const char *sep = " ", std::ostream &os = std::cout)
        {
            auto ptr = reinterpret_cast<const std::byte *>(&val);
            auto B(bytes_number<T>());

            for (size_t i(0); i < B; ++i)
            {
                os << "0o" << std::setw(3) << std::setfill('0') << std::oct << static_cast<int>(ptr[i]);
                if (i != B - 1)
                    os << sep;
            }
            os << std::dec;
        }

        template <typename T>
        void println_oct_bytes(const T &val, const char *sep = " ", std::ostream &os = std::cout)
        {
            print_oct_bytes(val, sep, os);
            os << std::endl;
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
        void println_dec_bytes(const T &val, const char *sep = " ", std::ostream &os = std::cout)
        {
            print_dec_bytes(val, sep, os);
            os << std::endl;
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
        template <typename T>
        void println_hex_bytes(const T &val, const char *sep = " ", std::ostream &os = std::cout)
        {
            print_hex_bytes(val, sep, os);
            os << std::endl;
        }

        // --------Modify methods--------

        template <typename T>
        void modify_bit(T &val, size_t bit_ind, bool bit_val)
        {
            if (bit_ind >= bits_number<T>())
                throw std::out_of_range("The bit index is out of range");

            auto ptr = reinterpret_cast<std::byte *>(&val);

            // Calculating the real position
            size_t byte_ind = bit_ind / BITS_PER_BYTE;
            size_t bit_ind_in_byte = BITS_PER_BYTE - 1 - bit_ind % BITS_PER_BYTE;

            unsigned char tmp = static_cast<unsigned char>(ptr[byte_ind]);

            if (tmp)
                tmp |= (1 << bit_ind_in_byte);
            else
                tmp &= ~(1 << bit_ind_in_byte);

            ptr[byte_ind] = static_cast<std::byte>(tmp);
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

            // Calculating the real position
            size_t byte_ind(bit_ind / BITS_PER_BYTE);
            size_t bit_ind_in_byte(BITS_PER_BYTE - 1 - bit_ind % BITS_PER_BYTE);

            return (static_cast<int>(ptr[byte_ind]) >> bit_ind_in_byte) & 1;
        }

        // --------Restore methods--------

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
            size_t bit_ind(0);

            while (beg != end)
            {
                bool bit_val = static_cast<bool>(*beg);

                if (bit_val)
                {
                    // Calculating the real position
                    size_t byte_ind = bit_ind / BITS_PER_BYTE;
                    size_t bit_ind_in_byte = BITS_PER_BYTE - 1 - bit_ind % BITS_PER_BYTE;

                    res |= (static_cast<T>(1) << (byte_ind * BITS_PER_BYTE + bit_ind_in_byte));
                }

                ++bit_ind;
                ++beg;
            }

            return res;
        }
        template <typename T>
        std::vector<std::byte> to_byte_vector(const T &data)
        {
            std::vector<std::byte> res;
            size_t B(bytes_number<T>());
            res.reserve(B);
            auto ptr = reinterpret_cast<const std::byte *>(&data);

            for (size_t i(0); i < B; ++i)
                res.push_back(ptr[i]);

            return res;
        }
        template <typename T>
        std::vector<bool> to_bit_vector(const T &data)
        {
            std::vector<bool> res;
            size_t B(bytes_number<T>());
            res.reserve(bits_number<T>());
            auto ptr = reinterpret_cast<const std::byte *>(&data);

            for (size_t i(0); i < B; ++i)
            {
                unsigned char byte = static_cast<unsigned char>(ptr[i]);
                for (size_t j(BITS_PER_BYTE); j-- > 0;)
                    res.push_back((byte >> j) & 1);
            }

            return res;
        }
    }

    // --------Reverse methods--------

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

    // --------Methods of bits number--------

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

    template <typename T>
    bool is_two_power(const T &val)
    {
        return val > 0 && (val & (val - 1)) == 0;
    }

    // --------Rotate methods--------

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

    namespace ECC
    {
        template <typename Container>
        size_t Haming_distance(const Container &c1, const Container &c2)
        {
            if (c1.size() != c2.size())
                throw std::invalid_argument("The containters must have the same size");

            size_t res(0);
            auto cit1 = c1.begin();
            auto cit2 = c2.begin();
            while (cit1 != c1.end())
            {
                if (*cit1 != *cit2)
                    ++res;

                ++cit1;
                ++cit2;
            }

            return res;
        }
        template <typename Container>
        size_t min_code_distance(const Container &codespace)
        {
            if (codespace.size() < 2)
                return 0;

            size_t res = std::numeric_limits<size_t>::max();

            for (auto it1 = codespace.begin(); it1 != codespace.end(); ++it1)
            {
                for (auto it2 = std::next(it1); it2 != codespace.end(); ++it2)
                {
                    size_t tmp = Haming_distance(*it1, *it2);
                    if (tmp < res)
                        res = tmp;
                }
            }

            return res;
        }

        bool can_detect_errors(size_t d, size_t g);
        bool can_correct_errors(size_t d, size_t g);

        struct ECCResult
        {
            std::vector<bool> data = std::vector<bool>{};
            bool error_detected = false;
            bool error_corrected = false;
            size_t errors_number = 0;
        };

        void print_report(const ECCResult &report, const char *sep = " ", std::ostream &os = std::cout);
        void println_report(const ECCResult &report, const char *sep = " ", std::ostream &os = std::cout);

        // --------Parity bit encode--------

        std::vector<bool> parity_bit_encode(const std::vector<bool> &bits, bool is_even = true);
        ECCResult parity_bit_decode(const std::vector<bool> &codeword, bool is_even = true);

        // --------Triple repeat encode--------

        std::vector<bool> triple_repeat_encode(const std::vector<bool> &bits);
        ECCResult triple_repeat_decode(const std::vector<bool> &codeword);

        // --------Hamming codes--------

        std::vector<std::vector<bool>> Hamming_matrix(size_t R);

        std::vector<bool> Hamming_encode(const std::vector<bool> &bits, size_t R);

        ECCResult Hamming_decode(const std::vector<bool> &codeword);

        // --------Reed-Muller codes--------

        // --------Hadamard codes--------

        std::vector<std::vector<short>> Hadamard_matrix(size_t n);

        std::vector<bool> Hadamard_encode(const std::vector<bool> &data);

    }
}

#endif