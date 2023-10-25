#pragma once

#include "solution.hpp"
#include "delta.hpp"
#include "random.hpp"
#include <vector>
#include <memory>

std::vector<int> getNeighborhoodOperations(const Solution &solution, int num_nodes);

int evaluateOperation(int operation, const Solution &solution, const Nodes &nodes, const DistanceMatrix &dist);
Solution applyOperation(int operation, Solution &solution);

class AbstractImprover
{
public:
    AbstractImprover() = default;
    virtual ~AbstractImprover() = default;

    virtual Solution improve(Solution &solution, const Nodes &nodes) = 0;
};

class GreedyImprover : public AbstractImprover
{
public:
    GreedyImprover() = default;
    ~GreedyImprover() = default;

    Solution improve(Solution &solution, const Nodes &nodes) override;

private:
    std::default_random_engine &m_rng = getRandomEngine();
};

class SteepestImprover : public AbstractImprover
{
public:
    SteepestImprover() = default;
    ~SteepestImprover() = default;

    Solution improve(Solution &solution, const Nodes &nodes) override;
};

std::unique_ptr<AbstractImprover> createImprover(char name);