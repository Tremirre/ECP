#include <cassert>
#include "random.hpp"

int main()
{
    auto rng = getRandomEngine();
    assert(shuffledIndices(0, rng).empty());

    auto indices = shuffledIndices(10, rng);
    assert(indices.size() == 10);

    for (std::size_t i = 0; i < indices.size(); ++i)
    {
        auto found = std::find(indices.begin(), indices.end(), i);
        assert(found != indices.end());
    }
}