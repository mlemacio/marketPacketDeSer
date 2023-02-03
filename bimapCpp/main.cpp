#include <iostream>
#include <vector>

#include "bimap/bimap.h"

int main()
{
    std::cout << "hello world";
    std::vector<std::pair<int, double>> pairs{
        {4, 3.4},
    };

    bimap_t bm(std::move(pairs));

    std::cout << bm.find(4) << std::endl;

    return 0;
}