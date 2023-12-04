#pragma once

#include <random>
#include <vector>

std::vector<int> shuffledIndices(
    std::size_t size,
    std::default_random_engine &rng);

std::default_random_engine &getRandomEngine();

std::vector<int> sampleWeightedWithoutReplacement(
    std::vector<int> &weights,
    int sample_size,
    std::default_random_engine &rng);