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

    virtual std::string additionalInfo() const { return ""; }

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

class CompositeImprover : public AbstractImprover
{
public:
    CompositeImprover(NeighborhoodType ntype, char improver_type, int param)
        : AbstractImprover(ntype), m_improver_type(improver_type), m_param(param) {}

protected:
    char m_improver_type;
    int m_param;
    std::default_random_engine &m_rng = getRandomEngine();
};

class MultipleStartImprover : public CompositeImprover
{
public:
    MultipleStartImprover(NeighborhoodType ntype, char improver_type, int param, int seed, int repeats)
        : CompositeImprover(ntype, improver_type, param), m_seed(seed), m_repeats(repeats) {}

    Solution improve(Solution &solution, const NodesDistPair &nodes) override;

protected:
    Solution randomizeSolution(const NodesDistPair &nodes, int seed_idx, int target_size) const;

    int m_repeats;
    int m_seed;

private:
    const int SEED_SPREAD = 1000;
};

class IteratedImprover : public CompositeImprover
{
public:
    IteratedImprover(NeighborhoodType ntype, char improver_type, int param, double time_limit, int perturb_size)
        : CompositeImprover(ntype, improver_type, param), m_time_limit(time_limit), m_perturb_size(perturb_size) {}

    Solution improve(Solution &solution, const NodesDistPair &nodes) override;

    std::string additionalInfo() const override;

protected:
    Solution peturb(Solution &solution, const NodesDistPair &nodes);

    double m_time_limit;
    int m_perturb_size;
    int m_iterations = 0;
};

class LargeNeighborhoodImprover : public CompositeImprover
{
public:
    LargeNeighborhoodImprover(NeighborhoodType ntype, char improver_type, int param, double time_limit, int disrupt_size)
        : CompositeImprover(ntype, improver_type, param), m_time_limit(time_limit), m_disrupt_size(disrupt_size) {}

    Solution improve(Solution &solution, const NodesDistPair &nodes) override;

    std::string additionalInfo() const override;

protected:
    // disrupt instead of destroy to avoid confusion with the destroy method
    virtual Solution disrupt(Solution &solution, const NodesDistPair &nodes);
    virtual Solution repair(Solution &solution, const NodesDistPair &nodes);

    double m_time_limit;
    int m_disrupt_size;
    int m_iterations = 0;
    std::default_random_engine &m_rng = getRandomEngine();
};

class GeneticLocalSearchImprover : public CompositeImprover
{
public:
    GeneticLocalSearchImprover(NeighborhoodType ntype, char improver_type, int param, double time_limit, int elite_size)
        : CompositeImprover(ntype, improver_type, param), m_time_limit(time_limit), m_elite_size(elite_size) {}

    Solution improve(Solution &solution, const NodesDistPair &nodes) override;

    std::string additionalInfo() const override;

protected:
    struct EvaluatedSolution
    {
        Solution solution;
        int cost;

        bool operator<(const EvaluatedSolution &other) const noexcept
        {
            return cost < other.cost;
        }
    };

    typedef std::vector<EvaluatedSolution> Population;

    virtual Solution recombine(Solution &s1, Solution &s2, const Nodes &nodes) const noexcept;
    virtual Population initializePopulation(const NodesDistPair &nodes) noexcept;

    double m_time_limit;
    int m_elite_size;
    int m_iterations = 0;
    std::default_random_engine &m_rng = getRandomEngine();
};

class AlternativeGeneticLocalSearchImprover : public GeneticLocalSearchImprover
{
public:
    AlternativeGeneticLocalSearchImprover(NeighborhoodType ntype, char improver_type, int param, double time_limit, int elite_size)
        : GeneticLocalSearchImprover(ntype, improver_type, param, time_limit, elite_size) {}

    Solution recombine(Solution &s1, Solution &s2, const Nodes &nodes) const noexcept override;
};

std::unique_ptr<AbstractImprover>
createImprover(
    char name,
    NeighborhoodType ntype,
    int param,
    char subname = 'p',
    double subparam_1 = 10.0,
    int subparam_2 = 10);
