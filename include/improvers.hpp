#pragma once

#include "solution.hpp"
#include "delta.hpp"
#include "random.hpp"
#include <vector>
#include <memory>

enum class NeighborhoodType
{
    NODE,
    EDGE,
    BOTH
};

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

    OpData(OperationType type, int i, int j)
        : m_type(type), m_first_idx(i), m_second_idx(j) {}

    OpData(int operation);
    int toInt() const;
    bool isInvalid() const;
    virtual bool isApplicable(const Solution &solution) const;
    int evaluate(const Solution &solution, const NodesDistPair &nodes) const;
    Solution apply(Solution &solution) const;
    void print() const;
};

std::vector<int> getNeighborhoodOperations(
    const Solution &solution,
    int num_nodes,
    NeighborhoodType type);

int evaluateOperation(int operation, const Solution &solution, const NodesDistPair &nodes);
Solution applyOperation(int operation, Solution &solution);

class AbstractImprover
{
public:
    AbstractImprover(NeighborhoodType ntype)
        : m_ntype(ntype) {}
    virtual ~AbstractImprover() = default;

    virtual Solution improve(Solution &solution, const NodesDistPair &nodes) = 0;

protected:
    NeighborhoodType m_ntype;

    virtual std::vector<int> generateOperationsVector(const Solution &solution, const NodesDistPair &nodes);
    virtual void updateOperationsVector(std::vector<int> &operations, const Solution &solution, const OpData &op) const;
};

class GreedyImprover : public AbstractImprover
{
public:
    GreedyImprover(NeighborhoodType ntype = NeighborhoodType::BOTH)
        : AbstractImprover(ntype) {}
    Solution improve(Solution &solution, const NodesDistPair &nodes) override;

private:
    std::default_random_engine &m_rng = getRandomEngine();
};

class SteepestImprover : public AbstractImprover
{
public:
    SteepestImprover(NeighborhoodType ntype)
        : AbstractImprover(ntype) {}
    Solution improve(Solution &solution, const NodesDistPair &nodes) override;

protected:
    virtual void updateOperationsVectorPostApply(std::vector<int> &operations, const Solution &solution, const OpData &op) const {};
};

class SteepestCandidateImprover : public SteepestImprover
{
public:
    SteepestCandidateImprover(NeighborhoodType ntype, int num_candidates)
        : SteepestImprover(ntype), m_num_candidates(num_candidates)
    {
    }

protected:
    std::vector<int> generateOperationsVector(const Solution &solution, const NodesDistPair &nodes) override;
    void updateOperationsVector(std::vector<int> &operations, const Solution &solution, const OpData &op) const override;
    void updateOperationsVectorPostApply(std::vector<int> &operations, const Solution &solution, const OpData &op) const override;

    std::vector<std::vector<int>> m_closest_nodes;
    int m_num_candidates;
};

class SteepestSimplePrioritizingImprover : public SteepestImprover
{
public:
    SteepestSimplePrioritizingImprover(NeighborhoodType ntype)
        : SteepestImprover(ntype)
    {
        if (ntype != NeighborhoodType::EDGE)
        {
            throw std::runtime_error("SteepestSimplePrioritizingImprover can only be used with edge neighborhood");
        }
    }

    Solution improve(Solution &solution, const NodesDistPair &nodes) override;
};

NeighborhoodType getNeighborhoodType(const std::string &ntype);

std::unique_ptr<AbstractImprover> createImprover(char name, NeighborhoodType ntype, int param);