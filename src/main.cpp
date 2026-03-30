#include <iostream>
#include "bytesbits.h"
#include <vector>

int main()
{
    std::vector<int> v1{1, 0, 1, 0, 1};
    std::vector<int> v2{1, 0, 0, 1, 1};

    std::cout << IMD::ECC::Haming_distance(v1, v2);

    std::cin.get();

    return 0;
}