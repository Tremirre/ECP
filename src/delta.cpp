#include "delta.hpp"

int getNodesSwapDelta(
    const NodesDistPair &nodes,
    const Solution &solution,
    int first_idx,
    int second_idx)
{
    if (first_idx == second_idx)
        return 0;

    if ((first_idx > second_idx) || (first_idx == 0 && second_idx == solution.size() - 1))
    {
        std::swap(first_idx, second_idx);
    }
    int first = solution[first_idx];
    int first_prev = solution[(first_idx - 1 + solution.size()) % solution.size()];
    int first_next = solution[(first_idx + 1) % solution.size()];
    int second = solution[second_idx];
    int second_prev = solution[(second_idx - 1 + solution.size()) % solution.size()];
    int second_next = solution[(second_idx + 1) % solution.size()];

    int delta = 0;
    delta += nodes.dist[first_prev][second];
    delta += nodes.dist[first][second_next];

    delta -= nodes.dist[first_prev][first];
    delta -= nodes.dist[second][second_next];

    if (first_next != second)
    {
        delta += nodes.dist[second][first_next];
        delta += nodes.dist[second_prev][first];
        delta -= nodes.dist[first][first_next];
        delta -= nodes.dist[second_prev][second];
    }

    return delta;
}

int getEdgesSwapDelta(
    const NodesDistPair &nodes,
    const Solution &solution,
    int first_idx,
    int second_idx)
{
    if (first_idx == second_idx)
        return 0;
    if (first_idx > second_idx || (first_idx == 0 && second_idx == solution.size() - 1))
    {
        std::swap(first_idx, second_idx);
    }
    int first = solution[first_idx];
    int first_next = solution[(first_idx + 1) % solution.size()];
    int second = solution[second_idx];
    int second_next = solution[(second_idx + 1) % solution.size()];

    if (first_next == second)
        return 0;

    int delta = 0;
    delta += nodes.dist[first][second];
    delta += nodes.dist[first_next][second_next];

    delta -= nodes.dist[first][first_next];
    delta -= nodes.dist[second][second_next];

    return delta;
}

int getReplaceNodeDelta(
    const NodesDistPair &nodes,
    const Solution &solution,
    int sol_idx,
    int node_idx)
{
    int prev = solution[(sol_idx - 1 + solution.size()) % solution.size()];
    int next = solution[(sol_idx + 1) % solution.size()];

    int delta = 0;
    delta += nodes.dist[prev][node_idx];
    delta += nodes.dist[node_idx][next];
    delta += nodes.nodes[node_idx].getWeight();
    delta -= nodes.nodes[solution[sol_idx]].getWeight();
    delta -= nodes.dist[prev][solution[sol_idx]];
    delta -= nodes.dist[solution[sol_idx]][next];

    return delta;
}