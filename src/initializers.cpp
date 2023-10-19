#include "initializers.hpp"

Solution initializeTwoCheapest(const Nodes &nodes, int start_idx, const DistanceMatrix &dist)
{
    Solution solution = {start_idx};
    int closest_idx = -1;
    int closest_dist = std::numeric_limits<int>::max();
    for (int i = 0; i < nodes.size(); ++i)
    {
        if (i == start_idx)
            continue;
        int distance = dist[start_idx][i] + nodes[i].getWeight();
        if (distance < closest_dist)
        {
            closest_dist = distance;
            closest_idx = i;
        }
    }
    solution.push_back(closest_idx);
    return solution;
}