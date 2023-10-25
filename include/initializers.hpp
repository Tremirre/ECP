#pragma once

#include "node.hpp"
#include "solution.hpp"
#include <functional>
#include <vector>
#include <optional>

Solution initializeTwoCheapest(
    const Nodes &nodes,
    int start_idx,
    const DistanceMatrix &dist);