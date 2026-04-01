#include "math_solver.h"

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