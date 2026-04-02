#include "bytesbits.h"
#include <algorithm>

#include <algorithm>
#include <typeinfo>
#include <cmath>
#include <utility>

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

std::vector<std::vector<short>> IMD::ECC::Hadamard_two_power_matrix(size_t n)
{
    if (!is_two_power(n))
        throw std::invalid_argument("The argument 'n' must be a power of 2");

    if (n == 1)
        return {{1}};

    size_t L(n / 2);
    auto H = IMD::ECC::Hadamard_two_power_matrix(L);
    std::vector<std::vector<short>> res(2 * L, std::vector<short>(2 * L));

    // Fill the four quadrants
    for (size_t i(0); i < L; ++i)
    {
        for (size_t j(0); j < L; ++j)
        {
            res[i][j] = H[i][j];          // top-left
            res[i][L + j] = H[i][j];      // top-right
            res[L + i][j] = H[i][j];      // bottom-left
            res[L + i][L + j] = -H[i][j]; // bottom-right
        }
    }

    return res;
}
std::vector<std::vector<short>> IMD::ECC::Paley_method(size_t n)
{
    size_t p(n - 1);

    if (!is_prime_number(p))
        throw std::invalid_argument("The parameter 'p' must be prime");
    if (p % 4 != 3)
        throw std::invalid_argument("The parameter 'p' must be equal 3 (mod 4)");

    std::vector<std::vector<short>> Q(p, std::vector<short>(p, 0));

    for (int i(0); i < p; ++i)
    {
        for (int j(0); j < p; ++j)
        {
            if (i == j)
                continue;

            int diff((j - i) % p);
            if (diff < 0)
                diff += p;

            Q[i][j] = is_quadratic_residue(diff, p) ? 1 : -1;
        }
    }

    std::vector<std::vector<short>> H(n, std::vector<short>(n, 0));

    for (int j(0); j < n; ++j)
        H[0][j] = 1;
    for (int i(0); i < n; ++i)
        H[i][0] = 1;

    for (int i(0); i < p; ++i)
        for (int j(0); j < p; ++j)
            H[i + 1][j + 1] = (i == j) ? -1 : Q[i][j];

    return H;
}

std::vector<std::vector<short>> IMD::ECC::Paley_double_method(size_t n)
{
    if (n % 2 != 0)
        throw std::invalid_argument("The argument 'n' must be even");

    size_t p(n / 2 - 1);

    if (!is_prime_number(p))
        throw std::invalid_argument("The parameter 'p' must be prime");
    if (p % 4 != 1)
        throw std::invalid_argument("The parameter 'p' must be equal 1 (mod 4)");

    std::vector<std::vector<short>> Q(p, std::vector<short>(p, 0));

    for (int i(0); i < p; ++i)
    {
        for (int j(0); j < p; ++j)
        {
            if (i == j)
                continue;

            int diff((j - i) % p);
            if (diff < 0)
                diff += p;

            Q[i][j] = is_quadratic_residue(diff, p) ? 1 : -1;
        }
    }

    size_t m(p + 1);
    std::vector<std::vector<short>> R(m, std::vector<short>(m, 0));

    for (size_t j = 0; j < m; ++j)
        R[0][j] = 1;
    for (size_t i = 0; i < m; ++i)
        R[i][0] = 1;

    for (size_t i(0); i < p; ++i)
        for (size_t j(0); j < p; ++j)
            R[i + 1][j + 1] = (i == j) ? -1 : Q[i][j];

    std::vector<std::vector<short>> H(n, std::vector<short>(n, 0));

    for (size_t i(0); i < m; ++i)
    {
        for (size_t j(0); j < m; ++j)
        {
            H[i][j] = R[i][j] + (i == j ? 1 : 0); // R+E

            H[i][j + m] = R[i][j] - (i == j ? 1 : 0); // R-E

            H[i + m][j] = R[i][j] - (i == j ? 1 : 0); // R-E

            H[i + m][j + m] = -R[i][j] - (i == j ? 1 : 0); // -R-E
        }
    }

    return H;
}

std::vector<std::vector<short>> IMD::ECC::Hadamard_matrix(size_t n)
{
    if (is_prime_number(n))
        return Hadamard_matrix(n);
    size_t p(n - 1);
    if (is_prime_number(p) && p % 4 == 3)
        return Paley_method(n);

    p = n / 2 - 1;
    if (is_prime_number(p) && p % 4 == 1)
        return Paley_double_method(n);

    throw std::runtime_error("There is no algorithm for creation of a Hadamard AArix");
}

std::vector<bool> IMD::ECC::Hadamard_encode(const std::vector<bool> &data)
{
    size_t K(data.size());
    size_t n(1 << (K - 1));

    bool is_invert(data.back());

    size_t row_idx(0);
    for (size_t i(0); i < K - 1; ++i)
        if (data[i])
            row_idx |= (1u << i);

    auto H = Hadamard_matrix(n);
    const auto &row = H[row_idx];

    std::vector<bool> codeword(n);
    for (size_t i(0); i < n; ++i)
    {
        codeword[i] = (row[i] == -1); // Translation +1 -> 0, -1 -> +1
        if (is_invert)
            codeword[i] = !codeword[i];
    }
    return codeword;
}

IMD::ECC::ECCResult IMD::ECC::Hadamard_decode(const std::vector<bool> &codeword)
{
    ECCResult res;
    res.error_detected = false;
    res.error_corrected = false;
    res.errors_number = 0;

    size_t n(codeword.size());
    size_t K(1 + log2(n));

    auto H = Hadamard_matrix(n);
    std::vector<int> corrs(2 * n);

    for (size_t i(0); i < n; ++i)
    {
        int corr(0);
        for (size_t k(0); k < n; ++k)
            corr += (codeword[k] ? -1 : 1) * H[i][k]; // Translation 0 -> +1, 1 -> -1

        corrs[i] = corr;
        corrs[i + n] = -corr;
    }
    int max_corr(-1);
    size_t corr_idx(0);

    for (size_t i(0); i < 2 * n; ++i)
    {
        int tmp = std::abs(corrs[i]);
        if (tmp > max_corr)
        {
            max_corr = tmp;
            corr_idx = i;
        }
    }

    int sign = (corrs[corr_idx] > 0 ? 1 : -1);
    bool invert(corr_idx >= n);
    size_t row_idx(corr_idx % n);

    std::vector<int> signal(n);
    for (size_t i(0); i < n; ++i)
        signal[i] = sign * H[row_idx][i];

    for (size_t i = 0; i < n; ++i)
    {
        if ((codeword[i] ? -1 : 1) != signal[i])
        {
            res.error_detected = true;
            ++res.errors_number;
        }
    }

    res.error_corrected = (res.errors_number > 0 && res.errors_number <= (n - 2) / 4);

    res.data.resize(K, false);
    res.data[K - 1] = invert;
    for (size_t i(0); i < K - 1; ++i)
        if (row_idx & (1u << i))
            res.data[i] = true;

    return res;
}

std::vector<std::vector<size_t>> IMD::ECC::monomials(size_t r, size_t m)
{
    std::vector<std::vector<size_t>> res{};

    for (size_t rr(0); rr <= r; ++rr)
    {
        std::vector<bool> term(m, false);
        std::fill(term.begin(), term.begin() + rr, true);

        do
        {
            std::vector<size_t> combination;
            for (size_t mm(0); mm < m; ++mm)
                if (term[mm])
                    combination.push_back(mm);

            res.push_back(combination);
        } while (std::prev_permutation(term.begin(), term.end()));
    }
    return res;
}

std::vector<bool> IMD::ECC::Reed_Muller_encode(const std::vector<bool> &data, size_t r, size_t m)
{
    auto monomial = monomials(r, m);

    size_t N(1 << m);
    size_t K = monomial.size();

    std::vector<bool> res(N, false);

    for (size_t t(0); t < N; ++t)
    {
        bool outcome(false);

        for (size_t kk(0); kk < K; ++kk)
        {
            if (!data[kk])
                continue;

            size_t L(monomial[kk].size());
            bool term(true);

            for (size_t i(0); i < L; ++i)
            {
                size_t idx = monomial[kk][i];
                if (!((t >> (m - idx - 1)) & 1))
                {
                    term = false;
                    break;
                }
            }

            if (term)
                outcome = !outcome; // XOR
        }

        res[t] = outcome;
    }
    return res;
}

IMD::ECC::ECCResult IMD::ECC::Reed_Muller_decode(const std::vector<bool> &codeword, size_t r, size_t m)
{
    ECCResult res;
    size_t N(1 << m);

    auto monomial = monomials(r, m);
    size_t K(monomial.size());

    if (codeword.size() != N)
    {
        res.error_detected = true;
        return res;
    }

    size_t d_min(1 << (m - r));
    size_t t((d_min - 1) / 2);

    // Sequence of all possible bit vectors
    std::vector<bool> best_data;
    size_t best_errors(N + 1);

    size_t total(1u << K);

    for (size_t tt(0); tt < total; ++tt)
    {
        std::vector<bool> data(K);
        for (size_t i(0); i < K; ++i)
            data[i] = (tt >> i) & 1;

        auto code = Reed_Muller_encode(data, r, m);

        size_t errors = Haming_distance(code, codeword);

        if (errors < best_errors)
        {
            best_errors = errors;
            best_data = data;

            if (errors <= t)
                break;
        }
    }

    res.data = best_data;
    res.errors_number = best_errors;
    res.error_detected = (best_errors > 0);
    res.error_corrected = (best_errors > 0 && best_errors <= t);

    return res;
}

bool IMD::is_prime_number(long long num)
{
    if (num <= 1)
        return false;
    if (num == 2)
        return true;
    if (num % 2 == 0)
        return false;

    for (long long i(3); i * i <= num; i += 2)
        if (num % i == 0)
            return false;

    return true;
}

bool IMD::is_quadratic_residue(long long a, long long p)
{
    if (a == 0)
        return false;
    for (long long i(1); i < p; ++i)
        if ((i * i) % p == a)
            return true;
    return false;
}