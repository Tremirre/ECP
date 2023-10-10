#include "random.hpp"

#include <algorithm>

std::vector<int> shuffledIndices(
    std::size_t size, std::default_random_engine &rng)
{
    std::vector<int> indices(size);
    for (int i = 0; i < size; i++)
    {
        indices[i] = i;
    }

    std::shuffle(std::begin(indices), std::end(indices), rng);
    return indices;
};

std::default_random_engine &getRandomEngine()
{
    static std::default_random_engine rng;
    return rng;
};