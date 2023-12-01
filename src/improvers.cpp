#include "improvers.hpp"
#include <cmath>
#include <iostream>
#include <queue>
#include <chrono>
#include <numeric>

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

int OpData::evaluate(const Solution &sol, const NodesDistPair &nodes) const
{
    switch (m_type)
    {
    case OperationType::NODE_SWAP:
        return getNodesSwapDelta(nodes, sol, m_first_idx, m_second_idx);
    case OperationType::EDGE_SWAP:
        return getEdgesSwapDelta(nodes, sol, m_first_idx, m_second_idx);
    case OperationType::NODE_REPLACE:
        return getReplaceNodeDelta(nodes, sol, m_first_idx, m_second_idx);
    default:
        throw std::runtime_error("Evaluate: Forbidden operation");
    }
}

bool OpData::isInvalid() const
{
    return m_type == OperationType::FORBIDDEN;
}

bool OpData::isApplicable(const Solution &solution) const
{
    if (m_type == OperationType::NODE_REPLACE)
    {
        bool replace_node_in_solution = std::find(solution.begin(), solution.end(), m_second_idx) != solution.end();
        return !replace_node_in_solution;
    }
    return true;
}

void OpData::print() const
{
    switch (m_type)
    {
    case OperationType::NODE_SWAP:
        std::cout << "Node Swap:\t";
        break;
    case OperationType::EDGE_SWAP:
        std::cout << "Edge Swap:\t";
        break;
    case OperationType::NODE_REPLACE:
        std::cout << "Node Replace:\t";
        break;
    default:
        std::cout << "FORBIDDEN:\t";
        break;
    }
    std::cout << m_first_idx << '\t' << m_second_idx << '\n';
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

std::vector<int> AbstractImprover::generateOperationsVector(const Solution &solution, const NodesDistPair &nodes)
{
    return getNeighborhoodOperations(solution, nodes.nodes.size(), m_ntype);
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

Solution GreedyImprover::improve(Solution &solution, const NodesDistPair &nodes)
{
    std::vector<int> operations = generateOperationsVector(solution, nodes);
    OpData selected_operation = OpData(OperationType::FORBIDDEN, 0, 0);

    while (true)
    {
        std::shuffle(operations.begin(), operations.end(), m_rng);
        selected_operation = OpData(OperationType::FORBIDDEN, 0, 0);
        for (int operation : operations)
        {
            OpData op(operation);
            int delta = op.evaluate(solution, nodes);

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

Solution SteepestImprover::improve(Solution &solution, const NodesDistPair &nodes)
{
    std::vector<int> operations = generateOperationsVector(solution, nodes);
    OpData best_op = OpData(OperationType::FORBIDDEN, 0, 0);

    while (true)
    {
        int best_delta = 0;
        best_op = OpData(OperationType::FORBIDDEN, 0, 0);

        for (int operation : operations)
        {
            OpData op(operation);
            int delta = op.evaluate(solution, nodes);

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

std::vector<int> SteepestCandidateImprover::generateOperationsVector(const Solution &solution, const NodesDistPair &nodes)
{
    if (m_num_candidates > nodes.nodes.size() - 1)
    {
        throw std::runtime_error("Too many candidates, max is " + std::to_string(nodes.nodes.size() - 1));
    }
    if (m_num_candidates < 1)
    {
        throw std::runtime_error("Too few candidates, min is 1");
    }
    if (m_ntype == NeighborhoodType::NODE)
    {
        throw std::runtime_error("SteepestCandidateImprover does not support NODE neighborhood type");
    }

    m_closest_nodes = std::vector<std::vector<int>>(nodes.nodes.size(), std::vector<int>(m_num_candidates, 0));
    for (int i = 0; i < nodes.nodes.size(); ++i)
    {
        std::vector<std::pair<int, int>> closest_nodes;
        for (int j = 0; j < nodes.nodes.size(); ++j)
        {
            if (i == j)
            {
                continue;
            }
            // TODO: check if this is correct
            closest_nodes.push_back(std::make_pair(nodes.dist[i][j] + nodes.nodes[j].getWeight(), j));
        }
        std::sort(closest_nodes.begin(), closest_nodes.end());
        for (int j = 0; j < m_num_candidates; ++j)
        {
            m_closest_nodes[i][j] = closest_nodes[j].second;
        }
    }
    std::vector<int> node_solution_index(nodes.nodes.size(), UNVISITED);
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

struct OperationDeltaPair
{
    int operation;
    int delta;
};

Solution SteepestSimplePrioritizingImprover::improve(Solution &solution, const NodesDistPair &nodes)
{
    auto cmp_op = [](const OperationDeltaPair o1, const OperationDeltaPair o2)
    {
        return o1.delta > o2.delta;
    };

    std::priority_queue<OperationDeltaPair, std::vector<OperationDeltaPair>, decltype(cmp_op)> operations_queue(cmp_op);
    std::vector<int> operations = generateOperationsVector(solution, nodes);

    for (int operation : operations)
    {
        int delta = OpData(operation).evaluate(solution, nodes);
        if (delta >= 0)
        {
            continue;
        }
        operations_queue.push({operation, delta});
    }

    Solution prev_solution = solution;

    while (!operations_queue.empty())
    {
        OperationDeltaPair pair = operations_queue.top();
        OpData op(pair.operation);
        operations_queue.pop();

        if (!op.isApplicable(solution))
        {
            continue;
        }
        int new_delta = op.evaluate(solution, nodes);
        if (new_delta >= 0 || new_delta != pair.delta)
        {
            continue;
        }

        prev_solution = solution;
        op.apply(solution);

        // readdding changed operations for reevaluation
        std::vector<int> nodes_outside_solution = findMissingNumbers(solution, nodes.nodes.size());

        // find all node replacements that have to be reevaluated
        for (int i = 0; i < solution.size(); i++)
        {
            int i_before = (i - 1 + solution.size()) % solution.size();
            int i_after = (i + 1) % solution.size();
            bool already_reevaluated = false;

            // for node replacements, add replace operations for the replaced node
            if (op.m_type == OperationType::NODE_REPLACE)
            {
                already_reevaluated = true;
                OpData replace_op(OperationType::NODE_REPLACE, i, prev_solution[op.m_first_idx]);
                int delta = replace_op.evaluate(solution, nodes);
                if (delta < 0)
                {
                    operations_queue.push({replace_op.toInt(), delta});
                }
            }

            if (
                solution[i] == prev_solution[i] &&
                solution[i_before] == prev_solution[i_before] &&
                solution[i_after] == prev_solution[i_after])
            {
                continue;
            }

            for (int j = 0; j < nodes_outside_solution.size(); j++)
            {
                if (already_reevaluated && nodes_outside_solution[j] == prev_solution[op.m_first_idx])
                {
                    continue;
                }
                OpData replace_op(OperationType::NODE_REPLACE, i, nodes_outside_solution[j]);
                int delta = replace_op.evaluate(solution, nodes);
                if (delta < 0)
                {
                    operations_queue.push({replace_op.toInt(), delta});
                }
            }
        }

        // find all edge swaps that have to be reevaluated
        for (int i = 0; i < solution.size(); ++i)
        {
            int i_successor = (i + 1) % solution.size();
            for (int j = i + 2; j < solution.size(); ++j)
            {
                if (i == 0 && j == solution.size() - 1)
                {
                    continue;
                }
                int j_successor = (j + 1) % solution.size();

                if (
                    solution[i] == prev_solution[i] &&
                    solution[j] == prev_solution[j] &&
                    solution[i_successor] == prev_solution[i_successor] &&
                    solution[j_successor] == prev_solution[j_successor])
                {
                    continue;
                }

                OpData edge_swap_op(OperationType::EDGE_SWAP, i, j);
                int delta = edge_swap_op.evaluate(solution, nodes);
                if (delta < 0)
                {
                    operations_queue.push({edge_swap_op.toInt(), delta});
                }
            }
        }
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

Solution MultipleStartImprover::randomizeSolution(const NodesDistPair &nodes, int seed_idx, int target_size) const
{
    m_rng.seed(m_seed + seed_idx * SEED_SPREAD);
    Solution solution = shuffledIndices(nodes.nodes.size(), m_rng);
    solution.resize(target_size);
    return solution;
}

Solution MultipleStartImprover::improve(Solution &solution, const NodesDistPair &nodes)
{
    Solution best_solution;
    int best_score = std::numeric_limits<int>::max();
    for (int i = 0; i < m_repeats; ++i)
    {
        Solution random_solution = randomizeSolution(nodes, i, solution.size());
        auto improver = createImprover(m_improver_type, m_ntype, m_param);
        Solution solution = improver->improve(random_solution, nodes);
        int score = evaluateSolution(nodes.nodes, solution);
        if (score < best_score)
        {
            best_score = score;
            best_solution = solution;
        }
    }
    return best_solution;
}

Solution IteratedImprover::peturb(Solution &solution, const NodesDistPair &nodes)
{
    for (int i = 0; i < m_perturb_size; ++i)
    {
        if (m_rng() % 2 == 0)
        {
            auto notesOutSolution = findMissingNumbers(solution, nodes.nodes.size());
            solution = replaceNode(solution, m_rng() % solution.size(), notesOutSolution[m_rng() % notesOutSolution.size()]);
        }
        else
        {
            solution = swapEdges(solution, m_rng() % solution.size(), m_rng() % solution.size());
        }
    }
    return solution;
}

Solution IteratedImprover::improve(Solution &solution, const NodesDistPair &nodes)
{
    Solution best_solution;
    int best_score = std::numeric_limits<int>::max();
    const auto start = std::chrono::high_resolution_clock::now();
    int iterations = 0;
    while (true)
    {
        Solution peturbed_solution = peturb(solution, nodes);
        auto improver = createImprover(m_improver_type, m_ntype, m_param);
        Solution solved_solution = improver->improve(peturbed_solution, nodes);
        int score = evaluateSolution(nodes.nodes, solved_solution);
        if (score < best_score)
        {
            best_score = score;
            best_solution = solved_solution;
        }

        const auto end = std::chrono::high_resolution_clock::now();
        const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        if (elapsed.count() > m_time_limit)
        {
            break;
        }
        iterations++;
    }
    std::cout << 'i' << iterations << '\n';
    return best_solution;
}

std::unique_ptr<AbstractImprover> createImprover(
    char name,
    NeighborhoodType ntype,
    int param,
    char subname,
    double subparam_1,
    int subparam_2)
{
    switch (name)
    {
    case 'g':
        return std::make_unique<GreedyImprover>(ntype);
    case 's':
        return std::make_unique<SteepestImprover>(ntype);
    case 'c':
        return std::make_unique<SteepestCandidateImprover>(ntype, param);
    case 'p':
        return std::make_unique<SteepestSimplePrioritizingImprover>(ntype);
    case 'm':
        return std::make_unique<MultipleStartImprover>(ntype, subname, param, subparam_1, subparam_2);
    case 'i':
        return std::make_unique<IteratedImprover>(ntype, subname, param, subparam_1, subparam_2);
    default:
        throw std::runtime_error("Invalid improver name");
    }
}