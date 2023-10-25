#include <algorithm>
#include <random>
#include <fstream>
#include <iostream>

#include "solvers.hpp"

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
    m_distances = calculateDistanceMatrix(nodes);
}

Solution GreedyCycleSolver::_solve(const Nodes &nodes, int start_idx, int visit_count)
{
    Solution solution = initializeTwoCheapest(nodes, start_idx, m_distances);
    std::vector<bool> visited(nodes.size(), false);
    visited[solution[0]] = true;
    visited[solution[1]] = true;

    while (solution.size() < visit_count)
    {
        int nearest_idx = -1;
        int added_idx = -1;
        int min_increase = std::numeric_limits<int>::max();

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

        solution.insert(solution.begin() + added_idx + 1, nearest_idx);
        visited[nearest_idx] = true;
    }

    return solution;
}

Solution GreedyTwoRegretSolver::_solve(const Nodes &nodes, int start_idx, int visit_count)
{
    Solution solution = initializeTwoCheapest(nodes, start_idx, m_distances);
    std::vector<bool> visited(nodes.size(), false);
    visited[solution[0]] = true;
    visited[solution[1]] = true;

    while (solution.size() < visit_count)
    {
        int nearest_idx = -1;
        int best_added_idx = -1;

        int max_regret = std::numeric_limits<int>::min();

        for (int i = 0; i < nodes.size(); ++i)
        {
            if (visited[i])
                continue;

            int added_idx = -1;
            int min_increase = std::numeric_limits<int>::max();
            int second_min_increase = std::numeric_limits<int>::max();

            for (int j = 0; j < solution.size(); ++j)
            {
                int next_idx = (j + 1) % solution.size();
                int increase = m_distances[solution[j]][i] + m_distances[i][solution[next_idx]];
                increase -= m_distances[solution[j]][solution[next_idx]];
                increase += nodes[i].getWeight();
                if (increase <= min_increase)
                {
                    second_min_increase = min_increase;
                    min_increase = increase;
                    added_idx = j;
                }
                else if (increase < second_min_increase)
                {
                    second_min_increase = increase;
                }
            }
            int regret = std::round(second_min_increase * m_second_weight) - std::round(min_increase * m_first_weight);
            if (regret > max_regret)
            {
                max_regret = regret;
                nearest_idx = i;
                best_added_idx = added_idx;
            }
        }

        solution.insert(solution.begin() + best_added_idx + 1, nearest_idx);
        visited[nearest_idx] = true;
    }
    return solution;
}

std::unique_ptr<AbstractSolver> createSolver(char name, double weigth1, double weight2)
{
    switch (name)
    {
    case 'r':
        return std::make_unique<RandomSolver>();
    case 'n':
        return std::make_unique<NearestNeighbourSolver>();
    case 'g':
        return std::make_unique<GreedyCycleSolver>();
    case 'd':
        return std::make_unique<GreedyTwoRegretSolver>(weigth1, weight2);
    default:
        throw std::runtime_error("Invalid solver name");
    }
}
