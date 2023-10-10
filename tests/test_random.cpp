#include <cassert>
#include "random.hpp"

void testShuffledIndicesZeroSizeReturnsEmpty()
{

    auto rng = std::default_random_engine{};
    assert(shuffledIndices(0, rng).empty());
}

void testShuffledIndicesReturnsVectorOfGivenSize()
{
    auto rng = std::default_random_engine{};
    int tested_sizes[] = {5, 10, 20, 100};
    for (auto size : tested_sizes)
    {
        assert(shuffledIndices(size, rng).size() == size);
    }
}

void testShuffledIndicesReturnsEveryIndex()
{
    auto rng = std::default_random_engine{};

    int tested_sizes[] = {5, 10, 20, 100};

    for (auto size : tested_sizes)
    {
        auto indices = shuffledIndices(size, rng);
        for (std::size_t i = 0; i < indices.size(); ++i)
        {
            auto found = std::find(indices.begin(), indices.end(), i);
            assert(found != indices.end());
        }
    }
}

int main()
{
    testShuffledIndicesZeroSizeReturnsEmpty();
    testShuffledIndicesReturnsVectorOfGivenSize();
    testShuffledIndicesReturnsEveryIndex();
}