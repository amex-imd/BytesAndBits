#include "bytesbits.h"

bool IMD::ECC::can_detect_errors(size_t d, size_t g)
{
    return d >= g + 1;
}
bool IMD::ECC::can_correct_errors(size_t d, size_t g)
{
    return d >= 2 * g + 1;
}

// --------Parity bit encode--------

std::vector<bool> IMD::ECC::parity_bit_encode(const std::vector<bool> &bits, bool is_even)
{
    std::vector<bool> res = bits;
    size_t ones(0);
    for (auto r : res)
        if (r)
            ++ones;

    bool parity_bit;

    if (is_even)
        parity_bit = (ones % 2 == 1);
    else
        parity_bit = (ones % 2 == 0);

    res.push_back(parity_bit);

    return res;
}
IMD::ECC::ECCResult IMD::ECC::parity_bit_decode(const std::vector<bool> &codeword, bool is_even)
{
    ECCResult res;

    if (codeword.empty())
    {
        res.error_detected = true;
        return res;
    }

    size_t L = codeword.size();
    size_t ones(0);
    bool real_parity_bit = codeword.back();

    for (size_t i(0); i < L - 1; ++i)
        if (codeword[i])
            ++ones;

    bool expected_parity_bit;
    if (is_even)
        expected_parity_bit = (ones % 2 == 1);
    else
        expected_parity_bit = (ones % 2 == 0);

    res.error_detected = (real_parity_bit != expected_parity_bit);
    if (res.error_detected)
        res.errors_number = 1;

    res.data = std::vector<bool>(codeword.begin(), codeword.end() - 1);

    return res;
}

// --------Triple repeat encode--------

std::vector<bool> IMD::ECC::triple_repeat_encode(const std::vector<bool> &bits)
{
    std::vector<bool> res;
    size_t L = bits.size();
    res.reserve(L * 3);

    for (size_t i(0); i < L; ++i)
    {
        bool bit = bits[i];

        res.push_back(bit);
        res.push_back(bit);
        res.push_back(bit);
    }

    return res;
}
IMD::ECC::ECCResult IMD::ECC::triple_repeat_decode(const std::vector<bool> &codeword)
{
    ECCResult res;

    if (codeword.size() % 3 != 0)
    {
        res.error_detected = true;
        return res;
    }

    size_t L = codeword.size() / 3;
    std::vector<bool> bits;
    bits.reserve(L);

    for (size_t i(0); i < L; ++i)
    {
        bool first = codeword[i * 3];
        bool second = codeword[i * 3 + 1];
        bool third = codeword[i * 3 + 2];

        if (first != second || first != third)
        {
            res.error_detected = true;
            res.error_corrected = true;
            ++res.errors_number;
        }

        size_t ones = (first ? 1 : 0) + (second ? 1 : 0) + (third ? 1 : 0);
        bool flag = ones >= 2;
        bits.push_back(flag);
    }

    res.data = bits;

    return res;
}

// --------Hamming codes--------

std::vector<std::vector<bool>> IMD::ECC::Hamming_matrix(size_t R)
{
    size_t N((1 << R) - 1);
    std::vector<std::vector<bool>> res(R, std::vector<bool>(N, false));

    for (size_t j(0); j < N; ++j)
        for (size_t i(0); i < R; ++i)
            res[i][j] = ((j + 1) >> (i)) & 1;

    return res;
}
std::vector<bool> IMD::ECC::Hamming_encode(const std::vector<bool> &bits, size_t R)
{
    size_t N = (1 << R) - 1;
    size_t K = N - R;

    if (bits.size() != K)
        throw std::invalid_argument("The number of data bits must equalt to " + std::to_string(K));

    std::vector<bool> res(N, false);

    // Putting of data bits
    size_t idx(0);
    for (size_t i(1); i <= N; ++i)
        if ((i & (i - 1)) != 0)
            res[i - 1] = bits[idx++];

    // Putting of control bits
    for (size_t i(0); i < R; ++i)
    {
        size_t r_pos(1 << i);
        bool tmp(0);
        for (size_t j(r_pos + 1); j <= N; ++j)
        {
            if ((j >> i) & 1)
                tmp ^= res[j - 1];
        }
        res[r_pos - 1] = tmp;
    }

    return res;
}
IMD::ECC::ECCResult IMD::ECC::Hamming_decode(const std::vector<bool> &codeword)
{
    ECCResult res;
    res.error_detected = false;
    res.error_corrected = false;
    res.errors_number = 0;

    size_t N = codeword.size();
    size_t R = 0;
    while ((1 << R) - 1 < N)
        ++R;

    if ((1 << R) - 1 != N)
        throw std::runtime_error("Invalid codeword length for Hamming code");

    size_t K = N - R;
    auto H = Hamming_matrix(R);
    std::vector<bool> s(R, false);

    for (size_t i(0); i < R; ++i)
    {
        bool tmp(0);
        for (size_t j(0); j < N; ++j)
        {
            if (H[i][j] && codeword[j])
                tmp = !tmp;
        }
        s[i] = tmp;
    }

    bool flag(true); // All good?
    for (bool ss : s)
    {
        if (ss)
        {
            flag = false;
            break;
        }
    }
    std::vector<bool> corrected = codeword;
    if (!flag)
    {
        res.error_detected = true;
        size_t error_pos(0);
        for (bool ss : s)
            error_pos = (error_pos << 1) | (ss ? 1 : 0);

        if (error_pos > 0 && error_pos <= N)
        {
            corrected[error_pos - 1] = !corrected[error_pos - 1];
            res.error_corrected = true;
            res.errors_number = 1;
        }
    }
    std::vector<bool> bits;
    bits.reserve(K);
    for (size_t i(1); i <= N; ++i)
    {
        if ((i & (i - 1)) != 0)
            bits.push_back(corrected[i - 1]);
    }

    res.data = bits;

    return res;
}

void IMD::ECC::print_report(const IMD::ECC::ECCResult &report, const char *sep, std::ostream &os)
{
    os << "Data: ";
    for (auto r : report.data)
        std::cout << r << " ";
    std::cout << std::endl
              << "Is error detected: " << report.error_detected << std::endl
              << "Is error corrected: " << report.error_corrected << std::endl
              << "Number of errors: " << report.errors_number << std::endl;
}
void IMD::ECC::println_report(const IMD::ECC::ECCResult &report, const char *sep, std::ostream &os)
{
    print_report(report, sep, os);
    os << std::endl;
}