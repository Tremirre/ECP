#pragma once
#include <vector>
#include <memory>
#include "random.hpp"
#include "node.hpp"

typedef std::vector<int> Solution;

void exportSolutionToFile(const Solution &solution, const std::string &filename);

int evaluateSolution(const Nodes &nodes, const Solution &solution);

class AbstractSolver
{
public:
    Solution solve(const Nodes &nodes, int start_idx, int visit_count);
    virtual ~AbstractSolver() = default;

private:
    virtual void beforeSolve(const Nodes &nodes, int start_idx){};
    virtual void afterSolve(const Nodes &nodes, int start_idx){};
    virtual Solution _solve(const Nodes &nodes, int start_idx, int visit_count) = 0;
};

class RandomSolver : public AbstractSolver
{
public:
    RandomSolver() = default;

private:
    Solution _solve(const Nodes &nodes, int start_idx, int visit_count) override;
    std::default_random_engine &m_rng = getRandomEngine();
};

class NearestNeighbourSolver : public AbstractSolver
{
public:
    NearestNeighbourSolver() = default;

private:
    void beforeSolve(const Nodes &nodes, int start_idx) override;
    Solution _solve(const Nodes &nodes, int start_idx, int visit_count) override;

protected:
    std::vector<std::vector<int>> m_distances;
};

class GreedyCycleSolver : public NearestNeighbourSolver
{
private:
    Solution _solve(const Nodes &nodes, int start_idx, int visit_count) override;
};

std::unique_ptr<AbstractSolver> createSolver(char name);