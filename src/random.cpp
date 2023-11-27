#include "random.hpp"

#include <algorithm>
#include <numeric>

std::vector<int> shuffledIndices(
    std::size_t size, std::default_random_engine &rng)
{
    std::vector<int> indices(size);
    std::iota(std::begin(indices), std::end(indices), 0);
    std::shuffle(std::begin(indices), std::end(indices), rng);
    return indices;
};

std::default_random_engine &getRandomEngine()
{
    static std::default_random_engine rng;
    return rng;
};
