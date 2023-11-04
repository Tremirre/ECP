#include "improvers.hpp"
#include <cmath>
#include <iostream>
#include <chrono>

constexpr static unsigned int VAL_BITS = 15;
constexpr static unsigned int TYPE_MASK = 0b11 << (VAL_BITS * 2);
constexpr static unsigned int FIRST_VAL_MASK = ((1 << VAL_BITS) - 1) << VAL_BITS;
constexpr static unsigned int SECOND_VAL_MASK = ~(TYPE_MASK | FIRST_VAL_MASK);
constexpr static int MAX_VAL = (1 << VAL_BITS) - 1;

enum class OperationType
{
    NODE_SWAP,
    EDGE_SWAP,
    NODE_REPLACE,
    FORBIDDEN
};

struct OpData
{
    OperationType m_type;
    int m_first_idx;
    int m_second_idx;

    OpData(int operation)
    {
        m_type = static_cast<OperationType>((operation & TYPE_MASK) >> (VAL_BITS * 2));
        m_first_idx = (operation & FIRST_VAL_MASK) >> VAL_BITS;
        m_second_idx = operation & SECOND_VAL_MASK;
    }

    OpData(OperationType type, int first_idx, int second_idx)
        : m_type(type), m_first_idx(first_idx), m_second_idx(second_idx)
    {
    }

    int toInt()
    {
        return (static_cast<int>(m_type) << (VAL_BITS * 2)) | (m_first_idx << VAL_BITS) | m_second_idx;
    }

    Solution apply(Solution &sol)
    {
        switch (m_type)
        {
        case OperationType::NODE_SWAP:
            return swapNodes(sol, m_first_idx, m_second_idx);
        case OperationType::EDGE_SWAP:
            return swapEdges(sol, m_first_idx, m_second_idx);
        case OperationType::NODE_REPLACE:
            return replaceNode(sol, m_first_idx, m_second_idx);
        default:
            throw std::runtime_error("Apply: Forbidden operation");
        }
    }

    int evaluate(const Solution &sol, const Nodes &nodes, const DistanceMatrix &dist)
    {
        switch (m_type)
        {
        case OperationType::NODE_SWAP:
            return getNodesSwapDelta(nodes, sol, dist, m_first_idx, m_second_idx);
        case OperationType::EDGE_SWAP:
            return getEdgesSwapDelta(nodes, sol, dist, m_first_idx, m_second_idx);
        case OperationType::NODE_REPLACE:
            return getReplaceNodeDelta(nodes, sol, dist, m_first_idx, m_second_idx);
        default:
            throw std::runtime_error("Evaluate: Forbidden operation");
        }
    }

    bool isInvalid()
    {
        return m_type == OperationType::FORBIDDEN;
    }
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

std::vector<int> getNeighborhoodOperations(
    const Solution &solution,
    int num_nodes,
    NeighborhoodType type)
{
    if (num_nodes > MAX_VAL)
    {
        throw std::runtime_error("Too many nodes, max is " + std::to_string(MAX_VAL));
    }

    std::vector<int> nodes_outside_solution = findMissingNumbers(solution, num_nodes);

    int num_operations = 0;

    int s_choose_2 = (solution.size() * (solution.size() - 1)) / 2;

    if (type != NeighborhoodType::EDGE)
    {
        // |s| choose 2 for node swaps
        num_operations += s_choose_2;
    }

    if (type != NeighborhoodType::NODE)
    {
        // |s| choose 2 - |s| for edge swaps
        num_operations += s_choose_2 - solution.size();
    }

    // |s| * |n| for node replacements
    num_operations += solution.size() * nodes_outside_solution.size();

    int invalid_operation = static_cast<int>(OperationType::FORBIDDEN) << (VAL_BITS * 2);
    std::vector<int> operations(num_operations, invalid_operation);

    int op_idx = -1;

    if (type != NeighborhoodType::EDGE)
    {

        for (int i = 0; i < solution.size(); ++i)
        {
            for (int j = i + 1; j < solution.size(); ++j)
            {
                OpData op(OperationType::NODE_SWAP, i, j);
                operations[++op_idx] = op.toInt();
            }
        }
    }
    if (type != NeighborhoodType::NODE)
    {
        for (int i = 0; i < solution.size(); ++i)
        {
            for (int j = i + 2; j < solution.size(); ++j)
            {
                if (i == 0 && j == solution.size() - 1)
                {
                    continue;
                }
                OpData op(OperationType::EDGE_SWAP, i, j);
                operations[++op_idx] = op.toInt();
            }
        }
    }

    for (int i = 0; i < solution.size(); ++i)
    {
        for (int j : nodes_outside_solution)
        {
            OpData op(OperationType::NODE_REPLACE, i, j);
            operations[++op_idx] = op.toInt();
        }
    }

    return operations;
}

void updateOperationsVector(
    std::vector<int> &operations,
    const Solution &solution,
    const OpData &selected_operation)
{
    if (selected_operation.m_type != OperationType::NODE_REPLACE)
    {
        return;
    }
    int new_node = selected_operation.m_second_idx;
    int old_node = solution[selected_operation.m_first_idx];
    int op_node;
    OperationType op_type;
    for (int i = 0; i < operations.size(); ++i)
    {
        op_node = operations[i] & SECOND_VAL_MASK;
        op_type = static_cast<OperationType>((operations[i] & TYPE_MASK) >> (VAL_BITS * 2));
        if (op_node == new_node && op_type == OperationType::NODE_REPLACE)
        {
            operations[i] = (operations[i] & ~SECOND_VAL_MASK) | old_node;
        }
    }
}

Solution GreedyImprover::improve(Solution &solution, const Nodes &nodes)
{
    DistanceMatrix dist = calculateDistanceMatrix(nodes);
    std::vector<int> operations = getNeighborhoodOperations(solution, nodes.size(), m_ntype);
    OpData selected_operation = OpData(OperationType::FORBIDDEN, 0, 0);

    while (true)
    {
        std::shuffle(operations.begin(), operations.end(), m_rng);
        selected_operation = OpData(OperationType::FORBIDDEN, 0, 0);
        for (int operation : operations)
        {
            OpData op(operation);
            int delta = op.evaluate(solution, nodes, dist);

            if (delta < 0)
            {
                selected_operation = op;
                break;
            }
        }
        if (selected_operation.isInvalid())
        {
            break;
        }

        updateOperationsVector(operations, solution, selected_operation);
        selected_operation.apply(solution);
    }
    return solution;
}

Solution SteepestImprover::improve(Solution &solution, const Nodes &nodes)
{
    DistanceMatrix dist = calculateDistanceMatrix(nodes);
    std::vector<int> operations = getNeighborhoodOperations(solution, nodes.size(), m_ntype);
    OpData best_op = OpData(OperationType::FORBIDDEN, 0, 0);

    while (true)
    {
        int best_delta = 0;
        best_op = OpData(OperationType::FORBIDDEN, 0, 0);

        for (int operation : operations)
        {
            OpData op(operation);
            int delta = op.evaluate(solution, nodes, dist);

            if (delta < best_delta)
            {
                best_delta = delta;
                best_op = op;
            }
        }

        if (best_op.isInvalid())
        {
            break;
        }
        updateOperationsVector(operations, solution, best_op);
        best_op.apply(solution);
    }

    return solution;
}

NeighborhoodType getNeighborhoodType(const std::string &ntype)
{
    if (ntype == "node")
    {
        return NeighborhoodType::NODE;
    }
    else if (ntype == "edge")
    {
        return NeighborhoodType::EDGE;
    }
    else if (ntype == "both")
    {
        return NeighborhoodType::BOTH;
    }
    else
    {
        throw std::runtime_error("Invalid neighborhood type");
    }
}

std::unique_ptr<AbstractImprover> createImprover(char name, NeighborhoodType ntype)
{
    switch (name)
    {
    case 'g':
        return std::make_unique<GreedyImprover>(ntype);
    case 's':
        return std::make_unique<SteepestImprover>(ntype);
    default:
        throw std::runtime_error("Invalid improver name");
    }
}