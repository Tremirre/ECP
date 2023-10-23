#include "solution.hpp"

#include <fstream>

int evaluateSolution(const Nodes &nodes, const Solution &solution)
{
    int score = 0;
    int distance;
    for (int i = 0; i < solution.size() - 1; ++i)
    {
        distance = nodes[solution[i]].distanceTo(nodes[solution[i + 1]]);
        score += distance + nodes[solution[i]].getWeight();
    }
    distance = nodes[solution[solution.size() - 1]].distanceTo(nodes[solution[0]]);
    score += distance + nodes[solution[solution.size() - 1]].getWeight();
    return score;
}

void exportSolutionToFile(
    const Solution &solution,
    const std::string &filename,
    int score)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file");
    }
    for (int i = 0; i < solution.size(); ++i)
    {
        file << solution[i] << '\n';
    }
    file << score << '\n';
}

Solution importSolutionFromFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file");
    }
    Solution solution;
    int node_idx;
    while (file >> node_idx)
    {
        solution.push_back(node_idx);
    }
    // Remove score
    solution.pop_back();
    return solution;
}

Solution swapNodes(Solution &solution, int first_idx, int second_idx)
{
    std::swap(solution[first_idx], solution[second_idx]);
    return solution;
}

Solution replaceNode(Solution &solution, int sol_idx, int node_idx)
{
    solution[sol_idx] = node_idx;
    return solution;
}

Solution swapEdges(Solution &solution, int first_idx, int second_idx)
{
    int idx_diff = std::abs(first_idx - second_idx);
    if (
        idx_diff < 2 ||
        idx_diff == solution.size() - 1)
    {
        return solution;
    }

    if (first_idx > second_idx)
    {
        std::swap(first_idx, second_idx);
    }

    std::reverse(solution.begin() + first_idx + 1, solution.begin() + second_idx + 1);

    return solution;
}