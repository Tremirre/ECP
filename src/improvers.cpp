#include "improvers.hpp"
#include <cmath>

constexpr static unsigned int VAL_BITS = 15;
constexpr static unsigned int TYPE_MASK = 0b11 << (VAL_BITS * 2);
constexpr static unsigned int FIRST_VAL_MASK = ((1 << VAL_BITS) - 1) << VAL_BITS;
constexpr static unsigned int SECOND_VAL_MASK = ~(TYPE_MASK | FIRST_VAL_MASK);
constexpr static int MAX_VAL = (1 << VAL_BITS) - 1;

enum class OperationType
{
    NODE_SWAP,
    EDGE_SWAP,
    NODE_REPLACE
};

std::vector<int> findMissingNumbers(const std::vector<int> &A, int N)
{
    std::vector<int> missing_numbers;
    std::vector<bool> present(N, false);

    for (int num : A)
    {
        present[num] = true;
    }

    for (int i = 0; i < N; i++)
    {
        if (!present[i])
        {
            missing_numbers.push_back(i);
        }
    }

    return missing_numbers;
}

std::vector<int> getNeighborhoodOperations(const Solution &solution, int num_nodes)
{
    if (num_nodes > MAX_VAL)
    {
        throw std::runtime_error("Too many nodes, max is " + std::to_string(MAX_VAL));
    }

    std::vector<int> nodes_outside_solution = findMissingNumbers(solution, num_nodes);

    // |s| choose 2 for node swaps
    int num_operations = (solution.size() * (solution.size() - 1)) / 2;

    // |s| choose 2 - |s| for edge swaps
    num_operations += num_operations - solution.size();

    // |s| * |n| for node replacements
    num_operations += solution.size() * nodes_outside_solution.size();

    std::vector<int> operations(num_operations);

    for (int i = 0; i < solution.size(); ++i)
    {
        for (int j = i + 1; j < solution.size(); ++j)
        {
            operations.push_back(
                static_cast<int>(OperationType::NODE_SWAP) << (VAL_BITS * 2) |
                (i << VAL_BITS) |
                j);
        }
    }

    for (int i = 0; i < solution.size(); ++i)
    {
        for (int j = i + 2; j < solution.size(); ++j)
        {
            if (i == 0 && j == solution.size() - 1)
            {
                continue;
            }
            operations.push_back(
                static_cast<int>(OperationType::EDGE_SWAP) << (VAL_BITS * 2) |
                (i << VAL_BITS) |
                j);
        }
    }

    for (int i = 0; i < solution.size(); ++i)
    {
        for (int j : nodes_outside_solution)
        {
            operations.push_back(
                static_cast<int>(OperationType::NODE_REPLACE) << (VAL_BITS * 2) |
                (i << VAL_BITS) |
                j);
        }
    }

    return operations;
}

int evaluateOperation(int operation, const Solution &solution, const Nodes &nodes, const DistanceMatrix &dist)
{
    int type = (operation & TYPE_MASK) >> (VAL_BITS * 2);
    int first_idx = (operation & FIRST_VAL_MASK) >> VAL_BITS;
    int second_idx = operation & SECOND_VAL_MASK;

    switch (static_cast<OperationType>(type))
    {
    case OperationType::NODE_SWAP:
        return getNodesSwapDelta(nodes, solution, dist, first_idx, second_idx);
    case OperationType::EDGE_SWAP:
        return getEdgesSwapDelta(nodes, solution, dist, first_idx, second_idx);
    case OperationType::NODE_REPLACE:
        return getReplaceNodeDelta(nodes, solution, dist, first_idx, second_idx);
    default:
        throw std::runtime_error("Invalid operation type");
    }
}

Solution applyOperation(int operation, Solution &solution)
{
    int type = (operation & TYPE_MASK) >> (VAL_BITS * 2);
    int first_idx = (operation & FIRST_VAL_MASK) >> VAL_BITS;
    int second_idx = operation & SECOND_VAL_MASK;

    switch (static_cast<OperationType>(type))
    {
    case OperationType::NODE_SWAP:
        return swapNodes(solution, first_idx, second_idx);
    case OperationType::EDGE_SWAP:
        return swapEdges(solution, first_idx, second_idx);
    case OperationType::NODE_REPLACE:
        return replaceNode(solution, first_idx, second_idx);
    default:
        throw std::runtime_error("Invalid operation type");
    }
}

Solution GreedyImprover::improve(Solution &solution, const Nodes &nodes)
{
    DistanceMatrix dist = calculateDistanceMatrix(nodes);
    while (true)
    {
        std::vector<int> operations = getNeighborhoodOperations(solution, nodes.size());
        std::shuffle(operations.begin(), operations.end(), m_rng);
        int selected_operation = -1;
        for (int operation : operations)
        {
            int delta = evaluateOperation(operation, solution, nodes, dist);

            if (delta < 0)
            {
                selected_operation = operation;
                break;
            }
        }
        if (selected_operation == -1)
        {
            break;
        }

        applyOperation(selected_operation, solution);
    }
    return solution;
}

Solution SteepestImprover::improve(Solution &solution, const Nodes &nodes)
{
    DistanceMatrix dist = calculateDistanceMatrix(nodes);
    while (true)
    {
        std::vector<int> operations = getNeighborhoodOperations(solution, nodes.size());
        int best_delta = 0;
        int best_operation = -1;

        for (int operation : operations)
        {
            int delta = evaluateOperation(operation, solution, nodes, dist);

            if (delta < best_delta)
            {
                best_delta = delta;
                best_operation = operation;
            }
        }

        if (best_operation == -1)
        {
            break;
        }

        applyOperation(best_operation, solution);
    }
    return solution;
}

std::unique_ptr<AbstractImprover> createImprover(char name)
{
    switch (name)
    {
    case 'g':
        return std::make_unique<GreedyImprover>();
    case 's':
        return std::make_unique<SteepestImprover>();
    default:
        throw std::runtime_error("Invalid improver name");
    }
}