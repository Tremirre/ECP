#pragma once

#include <functional>
#include <vector>
#include <optional>
#include "node.hpp"

typedef std::vector<int> Solution;
typedef std::vector<std::vector<int>> DistanceMatrix;

Solution initializeTwoCheapest(
    const Nodes &nodes,
    int start_idx,
    const DistanceMatrix &dist);