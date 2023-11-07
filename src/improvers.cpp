#include "improvers.hpp"
#include <cmath>
#include <iostream>
#include <chrono>

constexpr static unsigned int VAL_BITS = 15;
constexpr static unsigned int TYPE_MASK = 0b11 << (VAL_BITS * 2);
constexpr static unsigned int FIRST_VAL_MASK = ((1 << VAL_BITS) - 1) << VAL_BITS;
constexpr static unsigned int SECOND_VAL_MASK = ~(TYPE_MASK | FIRST_VAL_MASK);
constexpr static int MAX_VAL = (1 << VAL_BITS) - 1;

OpData::OpData(int operation)
{
    m_type = static_cast<OperationType>((operation & TYPE_MASK) >> (VAL_BITS * 2));
    m_first_idx = (operation & FIRST_VAL_MASK) >> VAL_BITS;
    m_second_idx = operation & SECOND_VAL_MASK;
}

int OpData::toInt() const
{
    return (static_cast<int>(m_type) << (VAL_BITS * 2)) | (m_first_idx << VAL_BITS) | m_second_idx;
}

Solution OpData::apply(Solution &sol) const
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

int OpData::evaluate(const Solution &sol, const Nodes &nodes, const DistanceMatrix &dist) const
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

bool OpData::isInvalid() const
{
    return m_type == OperationType::FORBIDDEN;
}

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

std::vector<int> AbstractImprover::generateOperationsVector(const Solution &solution, const Nodes &nodes, const DistanceMatrix &dist)
{
    return getNeighborhoodOperations(solution, nodes.size(), m_ntype);
}

void AbstractImprover::updateOperationsVector(
    std::vector<int> &operations,
    const Solution &solution,
    const OpData &selected_operation) const
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
    std::vector<int> operations = generateOperationsVector(solution, nodes, dist);
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
    std::vector<int> operations = generateOperationsVector(solution, nodes, dist);
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
        updateOperationsVectorPostApply(operations, solution, best_op);
    }

    return solution;
}

const int UNVISITED = -1;

std::vector<int> SteepestCandidateImprover::generateOperationsVector(const Solution &solution, const Nodes &nodes, const DistanceMatrix &dist)
{
    if (m_num_candidates > nodes.size() - 1)
    {
        throw std::runtime_error("Too many candidates, max is " + std::to_string(nodes.size() - 1));
    }
    if (m_num_candidates < 1)
    {
        throw std::runtime_error("Too few candidates, min is 1");
    }
    if (m_ntype == NeighborhoodType::NODE)
    {
        throw std::runtime_error("SteepestCandidateImprover does not support NODE neighborhood type");
    }

    m_closest_nodes = std::vector<std::vector<int>>(nodes.size(), std::vector<int>(m_num_candidates, 0));
    for (int i = 0; i < nodes.size(); ++i)
    {
        std::vector<std::pair<int, int>> closest_nodes;
        for (int j = 0; j < nodes.size(); ++j)
        {
            if (i == j)
            {
                continue;
            }
            // TODO: check if this is correct
            closest_nodes.push_back(std::make_pair(dist[i][j] + nodes[j].getWeight(), j));
        }
        std::sort(closest_nodes.begin(), closest_nodes.end());
        for (int j = 0; j < m_num_candidates; ++j)
        {
            m_closest_nodes[i][j] = closest_nodes[j].second;
        }
    }
    std::vector<int> node_solution_index(nodes.size(), UNVISITED);
    for (int i = 0; i < solution.size(); ++i)
    {
        node_solution_index[solution[i]] = i;
    }

    std::vector<int> operations = std::vector<int>(m_num_candidates * solution.size() * 2, 0);

    int op_idx = -1;
    for (int i = 0; i < solution.size(); ++i)
    {
        int i_successor = (i + 1) % solution.size();
        int i_predecessor = (i - 1 + solution.size()) % solution.size();
        for (int j = 0; j < m_num_candidates; ++j)
        {
            int candidate = m_closest_nodes[solution[i]][j];
            int candidate_solution_index = node_solution_index[candidate];
            if (candidate_solution_index == UNVISITED)
            {
                operations[++op_idx] = OpData(OperationType::NODE_REPLACE, i_successor, candidate).toInt();
                operations[++op_idx] = OpData(OperationType::NODE_REPLACE, i_predecessor, candidate).toInt();
            }
            else
            {
                int candidate_predecessor = (candidate_solution_index - 1 + solution.size()) % solution.size();
                operations[++op_idx] = OpData(OperationType::EDGE_SWAP, i, candidate_solution_index).toInt();
                operations[++op_idx] = OpData(OperationType::EDGE_SWAP, i_predecessor, candidate_predecessor).toInt();
            }
        }
    }

    return operations;
}

void SteepestCandidateImprover::updateOperationsVector(std::vector<int> &operations, const Solution &solution, const OpData &op) const
{
}

void SteepestCandidateImprover::updateOperationsVectorPostApply(std::vector<int> &operations, const Solution &solution, const OpData &op) const
{
    std::vector<int> node_solution_index(200, UNVISITED);
    for (int i = 0; i < solution.size(); ++i)
    {
        node_solution_index[solution[i]] = i;
    }

    int op_idx = -1;
    for (int i = 0; i < solution.size(); ++i)
    {
        int i_successor = (i + 1) % solution.size();
        int i_predecessor = (i - 1 + solution.size()) % solution.size();
        for (int j = 0; j < m_num_candidates; ++j)
        {
            int candidate = m_closest_nodes[solution[i]][j];
            int candidate_solution_index = node_solution_index[candidate];
            if (candidate_solution_index == UNVISITED)
            {
                operations[++op_idx] = OpData(OperationType::NODE_REPLACE, i_successor, candidate).toInt();
                operations[++op_idx] = OpData(OperationType::NODE_REPLACE, i_predecessor, candidate).toInt();
            }
            else
            {
                int candidate_predecessor = (candidate_solution_index - 1 + solution.size()) % solution.size();
                operations[++op_idx] = OpData(OperationType::EDGE_SWAP, i, candidate_solution_index).toInt();
                operations[++op_idx] = OpData(OperationType::EDGE_SWAP, i_predecessor, candidate_predecessor).toInt();
            }
        }
    }
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

std::unique_ptr<AbstractImprover> createImprover(char name, NeighborhoodType ntype, int param)
{
    switch (name)
    {
    case 'g':
        return std::make_unique<GreedyImprover>(ntype);
    case 's':
        return std::make_unique<SteepestImprover>(ntype);
    case 'c':
        return std::make_unique<SteepestCandidateImprover>(ntype, param);
    default:
        throw std::runtime_error("Invalid improver name");
    }
}