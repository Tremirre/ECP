#pragma once

#include "improvers.hpp"

int getNodesSwapDelta(
    const Nodes &nodes,
    const Solution &solution,
    const DistanceMatrix &dist,
    int first_idx,
    int second_idx);

int getEdgesSwapDelta(
    const Nodes &nodes,
    const Solution &solution,
    const DistanceMatrix &dist,
    int first_idx,
    int second_idx);

int getReplaceNodeDelta(
    const Nodes &nodes,
    const Solution &solution,
    const DistanceMatrix &dist,
    int sol_idx, int node_idx);
