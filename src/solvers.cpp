#include <algorithm>
#include <random>
#include <fstream>
#include <iostream>

#include "solvers.hpp"

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
    score += distance;
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

void RandomSolver::beforeSolve(const Nodes &nodes, int start_idx)
{
    m_rng.seed(start_idx + 1000);
}

Solution NearestNeighbourSolver::_solve(const Nodes &nodes, int start_idx, int visit_count)
{
    Solution solution;
    solution.push_back(start_idx);
    std::vector<bool> visited(nodes.size(), false);
    visited[start_idx] = true;
    int current_idx = start_idx;

    for (int i = 0; i < visit_count - 1; ++i)
    {
        int min_score = std::numeric_limits<int>::max();
        int min_idx = -1;
        for (int j = 0; j < nodes.size(); ++j)
        {
            if (visited[j])
            {
                continue;
            }
            int score = m_distances[current_idx][j] + nodes[j].getWeight();
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
    m_distances.resize(nodes.size(), std::vector<int>(nodes.size(), 0));

    for (int i = 0; i < nodes.size(); ++i)
    {
        for (int j = i + 1; j < nodes.size(); ++j)
        {
            int distance = nodes[i].distanceTo(nodes[j]);
            m_distances[i][j] = distance;
            m_distances[j][i] = distance;
        }
    }
}

Solution GreedyCycleSolver::_solve(const Nodes &nodes, int start_idx, int visit_count)
{
    Solution solution;
    solution.push_back(start_idx);

    std::vector<bool> visited(nodes.size(), false);
    visited[start_idx] = true;

    while (solution.size() < visit_count)
    {
        int nearest_idx = -1;
        int added_idx = -1;
        int min_increase = std::numeric_limits<int>::max();

        if (solution.size() == 1)
        {
            for (int i = 0; i < nodes.size(); i++)
            {
                if (i == start_idx)
                    continue;

                int increase = nodes[start_idx].distanceTo(nodes[i]) + nodes[i].getWeight();
                if (increase < min_increase)
                {
                    nearest_idx = i;
                    min_increase = increase;
                }
            }
            solution.push_back(nearest_idx);
            visited[nearest_idx] = true;
            continue;
        }

        for (int i = 0; i < nodes.size(); ++i)
        {
            if (visited[i])
                continue;
            for (int j = 0; j < solution.size(); ++j)
            {
                int next_idx = (j + 1) % solution.size();
                int increase = m_distances[solution[j]][i] + m_distances[i][solution[next_idx]];
                increase -= m_distances[solution[j]][solution[next_idx]];
                increase += nodes[i].getWeight();
                if (increase < min_increase)
                {
                    min_increase = increase;
                    nearest_idx = i;
                    added_idx = j;
                }
            }
        }

        if (nearest_idx != -1)
        {
            solution.insert(solution.begin() + added_idx + 1, nearest_idx);
            visited[nearest_idx] = true;
        }
    }

    return solution;
}

std::unique_ptr<AbstractSolver> createSolver(char name)
{
    switch (name)
    {
    case 'r':
        return std::make_unique<RandomSolver>();
    case 'n':
        return std::make_unique<NearestNeighbourSolver>();
    case 'g':
        return std::make_unique<GreedyCycleSolver>();
    default:
        throw std::runtime_error("Invalid solver name");
    }
}
