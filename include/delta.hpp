#pragma once

#include "improvers.hpp"

int getNodesSwapDelta(
    const NodesDistPair &nodes,
    const Solution &solution,
    int first_idx,
    int second_idx);

int getEdgesSwapDelta(
    const NodesDistPair &nodes,
    const Solution &solution,
    int first_idx,
    int second_idx);

int getReplaceNodeDelta(
    const NodesDistPair &nodes,
    const Solution &solution,
    int sol_idx, int node_idx);
