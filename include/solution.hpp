#pragma once

#include "node.hpp"
#include <memory>
#include <string>

typedef std::vector<int> Solution;

int evaluateSolution(const Nodes &nodes, const Solution &solution);
void exportSolutionToFile(const Solution &solution, const std::string &filename, int score);
Solution importSolutionFromFile(const std::string &filename);

Solution swapNodes(Solution &solution, int first_idx, int second_idx);
Solution replaceNode(Solution &solution, int sol_idx, int node_idx);
Solution swapEdges(Solution &solution, int first_idx, int second_idx);
