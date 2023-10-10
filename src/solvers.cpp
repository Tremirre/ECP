#include <algorithm>
#include <random>
#include <fstream>

#include "solvers.hpp"

void exportSolutionToFile(const Solution &solution, const std::string &filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file");
    }
    for (int i = 0; i < solution.size(); i++)
    {
        file << solution[i] << '\n';
    }
}

int evaluateSolution(const Nodes &nodes, const Solution &solution)
{
    int score = 0;
    int distance;
    for (int i = 0; i < solution.size() - 1; i++)
    {
        distance = nodes[solution[i]].distanceTo(nodes[solution[i + 1]]);
        score += distance + nodes[solution[i]].getWeight();
    }
    return score;
}

Solution AbstractSolver::solve(const Nodes &nodes, int start_idx, int visit_count)
{
    beforeSolve(nodes, start_idx);
    Solution solution = _solve(nodes, start_idx, visit_count);
    afterSolve(nodes, start_idx);
    return solution;
}

Solution RandomSolver::_solve(const Nodes &nodes, int start_idx, int visit_count)
{
    Solution indices = shuffledIndices(nodes.size(), m_rng);
    indices.resize(visit_count);
    return indices;
}

Solution NearestNeighbourSolver::_solve(const Nodes &nodes, int start_idx, int visit_count)
{
    Solution solution;
    solution.push_back(start_idx);
    std::vector<bool> visited(nodes.size(), false);
    visited[start_idx] = true;
    int current_idx = start_idx;

    for (int i = 0; i < visit_count - 1; i++)
    {
        int min_score = std::numeric_limits<int>::max();
        int min_idx = -1;
        for (int j = 0; j < nodes.size(); j++)
        {
            if (visited[j])
            {
                continue;
            }
            int score = m_scores[current_idx][j];
            if (score < min_score)
            {
                min_score = score;
                min_idx = j;
            }
        }
        solution.push_back(min_idx);
        visited[min_idx] = true;
        current_idx = min_idx;
    }
    return solution;
}

void NearestNeighbourSolver::beforeSolve(const Nodes &nodes, int start_idx)
{
    m_scores.resize(nodes.size(), std::vector<int>(nodes.size(), 0));

    for (int i = 0; i < nodes.size(); i++)
    {
        for (int j = i + 1; j < nodes.size(); j++)
        {
            int distance = nodes[i].distanceTo(nodes[j]);
            m_scores[i][j] = distance + nodes[i].getWeight();
            m_scores[j][i] = distance + nodes[j].getWeight();
        }
    }
}

AbstractSolver *createSolver(char name)
{
    switch (name)
    {
    case 'r':
        return new RandomSolver();
    case 'n':
        return new NearestNeighbourSolver();
    default:
        throw std::runtime_error("Invalid solver name");
    }
}