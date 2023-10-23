#pragma once

#include <vector>
#include <memory>
#include "random.hpp"
#include "node.hpp"
#include "solution.hpp"
#include "initializers.hpp"

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
    void beforeSolve(const Nodes &nodes, int start_idx) override;
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

class GreedyTwoRegretSolver : public GreedyCycleSolver
{
public:
    GreedyTwoRegretSolver(double first_weight, double second_weight) : m_first_weight(first_weight), m_second_weight(second_weight) {}

private:
    Solution _solve(const Nodes &nodes, int start_idx, int visit_count) override;

    double m_first_weight;
    double m_second_weight;
};

std::unique_ptr<AbstractSolver> createSolver(char name, double weigth1, double weight2);