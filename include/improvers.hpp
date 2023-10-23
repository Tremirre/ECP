#pragma once

#include "solution.hpp"
#include <vector>
#include <memory>

class AbstractImprover
{
public:
    AbstractImprover() = default;
    virtual ~AbstractImprover() = default;

    virtual Solution improve(Solution &solution, const Nodes &nodes) = 0;
};

std::unique_ptr<AbstractImprover> createImprover(char name);