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

std::vector<int> getNeighborhoodOperations(
    const Solution &solution,
    int num_nodes,
    NeighborhoodType type);

int evaluateOperation(int operation, const Solution &solution, const Nodes &nodes, const DistanceMatrix &dist);
Solution applyOperation(int operation, Solution &solution);

class AbstractImprover
{
public:
    AbstractImprover(NeighborhoodType ntype)
        : m_ntype(ntype) {}
    virtual ~AbstractImprover() = default;

    virtual Solution improve(Solution &solution, const Nodes &nodes) = 0;

protected:
    NeighborhoodType m_ntype;
};

class GreedyImprover : public AbstractImprover
{
public:
    GreedyImprover(NeighborhoodType ntype = NeighborhoodType::BOTH)
        : AbstractImprover(ntype) {}
    Solution improve(Solution &solution, const Nodes &nodes) override;

private:
    std::default_random_engine &m_rng = getRandomEngine();
};

class SteepestImprover : public AbstractImprover
{
public:
    SteepestImprover(NeighborhoodType ntype)
        : AbstractImprover(ntype) {}
    Solution improve(Solution &solution, const Nodes &nodes) override;
};

NeighborhoodType getNeighborhoodType(const std::string &ntype);

std::unique_ptr<AbstractImprover> createImprover(char name, NeighborhoodType ntype);