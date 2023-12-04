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

std::vector<int> sampleWeightedWithoutReplacement(
    std::vector<int> &weights,
    int sample_size,
    std::default_random_engine &rng)
{
    if (sample_size > weights.size())
    {
        throw std::runtime_error("Sample size cannot be larger than the number of weights");
    }
    std::vector<int> samples;

    for (int i = 0; i < sample_size; ++i)
    {
        std::discrete_distribution<int> dist(std::begin(weights), std::end(weights));
        int sample = dist(rng);
        samples.push_back(sample);
        weights[sample] = 0;
    }

    return samples;
}