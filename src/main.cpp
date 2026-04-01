#include <iostream>
#include "bytesbits.h"
#include <vector>

int main()
{
    std::cout << "Number of bytes: " << IMD::bytes_number<short>() << std::endl;
    std::cout << "Number of bits: " << IMD::bits_number<short>() << std::endl;
    IMD::println_info<short>();

    short val = 314;

    std::cout << "BIG ENDIAN" << std::endl;

    IMD::BIG_ENDIAN::println_bits(val);
    IMD::BIG_ENDIAN::println_bin_bytes(val);
    IMD::BIG_ENDIAN::println_oct_bytes(val);
    IMD::BIG_ENDIAN::println_dec_bytes(val);
    IMD::BIG_ENDIAN::println_hex_bytes(val);

    for (size_t i(0); i < IMD::bits_number<short>(); ++i)
        std::cout << i << " <=> " << IMD::BIG_ENDIAN::get_bit(val, i) << std::endl;

    std::cout << "Byte vector: ";
    auto byte_vec = IMD::BIG_ENDIAN::to_byte_vector(val);
    for (auto byte : byte_vec)
        std::cout << (short)byte << " ";

    std::cout << std::endl;
    std::cout << "Bit vector: ";
    auto bit_vec = IMD::BIG_ENDIAN::to_bit_vector(val);
    for (auto bit : bit_vec)
        std::cout << (short)bit << " ";
    std::cout << std::endl;

    std::cout << "LITTLE ENDIAN" << std::endl;

    IMD::LITTLE_ENDIAN::println_bits(val);
    IMD::LITTLE_ENDIAN::println_bin_bytes(val);
    IMD::LITTLE_ENDIAN::println_oct_bytes(val);
    IMD::LITTLE_ENDIAN::println_dec_bytes(val);
    IMD::LITTLE_ENDIAN::println_hex_bytes(val);

    for (size_t i(0); i < IMD::bits_number<short>(); ++i)
        std::cout << i << " <=> " << IMD::LITTLE_ENDIAN::get_bit(val, i) << std::endl;

    std::cout << "Byte vector: ";
    byte_vec = IMD::LITTLE_ENDIAN::to_byte_vector(val);
    for (auto byte : byte_vec)
        std::cout << (short)byte << " ";

    std::cout << std::endl;
    std::cout << "Bit vector: ";
    bit_vec = IMD::LITTLE_ENDIAN::to_bit_vector(val);
    for (auto bit : bit_vec)
        std::cout << (short)bit << " ";
    std::cout << std::endl;

    std::cout << "ERROR CODE CORRECTING" << std::endl;

    std::cout << "SOURCE DATA: ";
    std::vector<bool> bits = {0, 1, 1, 0};
    for (auto b : bits)
        std::cout << b << " ";
    std::cout << std::endl;

    std::cout << "PARITY BIT" << std::endl;

    auto codeword = IMD::ECC::parity_bit_encode(bits);
    std::cout << "Codeword with no errors: ";
    for (auto x : codeword)
        std::cout << x << " ";
    std::cout << std::endl;
    auto report = IMD::ECC::parity_bit_decode(codeword);

    IMD::ECC::println_report(report);

    std::cout << "Codeword with an error: ";
    codeword[1] = 0;
    for (auto x : codeword)
        std::cout << x << " ";

    std::cout << std::endl;
    report = IMD::ECC::parity_bit_decode(codeword);

    IMD::ECC::println_report(report);

    std::cout << "TRIPLE REPEAT" << std::endl;
    codeword = IMD::ECC::triple_repeat_encode(bits);
    std::cout << "Codeword with no errors: ";
    for (auto x : codeword)
        std::cout << x << " ";
    std::cout << std::endl;

    report = IMD::ECC::triple_repeat_decode(codeword);
    IMD::ECC::println_report(report);

    std::cout << "Codeword with some errors: ";
    codeword[1] = 1;
    codeword[5] = 0;
    codeword[10] = 1;
    for (auto x : codeword)
        std::cout << x << " ";
    std::cout << std::endl;

    report = IMD::ECC::triple_repeat_decode(codeword);
    IMD::ECC::println_report(report);

    short R = 3;
    std::cout << "HAMMING CODE (R=" << R << ")" << std::endl;

    codeword = IMD::ECC::Hamming_encode(bits, R);
    std::cout << "Codeword with no errors: ";
    for (auto x : codeword)
        std::cout << x << " ";
    std::cout << std::endl;

    report = IMD::ECC::Hamming_decode(codeword);
    IMD::ECC::println_report(report);

    std::cout << "Codeword with an error: ";
    codeword[0] = 0;
    for (auto x : codeword)
        std::cout << x << " ";
    std::cout << std::endl;

    report = IMD::ECC::Hamming_decode(codeword);
    IMD::ECC::println_report(report);

    std::cout << "Hadamard codes" << std::endl;

    std::cout << "Hadamard matrix 12x12: " << std::endl;
    auto mrx = IMD::ECC::Hadamard_matrix(12);

    for (const auto &x : mrx)
    {
        for (auto y : x)
            std::cout << y << " ";
        std::cout << std::endl;
    }

    codeword = IMD::ECC::Hadamard_encode(bits);

    std::cout << "Codeword with no errors: ";
    for (auto x : codeword)
        std::cout << x << " ";
    std::cout << std::endl;

    report = IMD::ECC::Hadamard_decode(codeword);
    IMD::ECC::println_report(report);

    std::cout << "Codeword with an error: ";
    codeword[5] = 1;
    for (auto x : codeword)
        std::cout << x << " ";
    std::cout << std::endl;

    report = IMD::ECC::Hadamard_decode(codeword);
    IMD::ECC::println_report(report);

    std::cin.get();

    return 0;
}
