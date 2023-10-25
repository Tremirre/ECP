#include "solution.hpp"
#include "delta.hpp"
#include <cassert>
#include <iostream>

const static Nodes nodes = {
    {0, 0, 5},
    {1, 3, 12},
    {4, 2, 6},
    {5, 6, 12},
    {2, 4, 55},
    {7, 12, 1},
    {10, 3, 4},
    {4, 9, 34}};

const static DistanceMatrix dist = {
    {0, 3, 4, 8, 4, 14, 10, 10},
    {3, 0, 3, 5, 1, 11, 9, 7},
    {4, 3, 0, 4, 3, 10, 6, 7},
    {8, 5, 4, 0, 4, 6, 6, 3},
    {4, 1, 3, 4, 0, 9, 8, 5},
    {14, 11, 10, 6, 9, 0, 9, 4},
    {10, 9, 6, 6, 8, 9, 0, 8},
    {10, 7, 7, 3, 5, 4, 8, 0},
};

void testGetDeltaReplaceNode()
{
    // Note: operations are in-place
    Solution solution = {0, 1, 2, 3, 4};

    assert(getReplaceNodeDelta(nodes, solution, dist, 2, 7) == 31);
    assert(getReplaceNodeDelta(nodes, solution, dist, 2, 2) == 0);
    assert(getReplaceNodeDelta(nodes, solution, dist, 0, 6) == 9);

    int original_evaluation = evaluateSolution(nodes, solution);
    int delta = getReplaceNodeDelta(nodes, solution, dist, 3, 7);
    replaceNode(solution, 3, 7);
    int new_evaluation = evaluateSolution(nodes, solution);
    assert(new_evaluation - original_evaluation == delta);
}

void testGetDeltaSwapNodes()
{
    // Note: operations are in-place
    Solution solution = {0, 1, 2, 3, 4};

    assert(getNodesSwapDelta(nodes, solution, dist, 1, 1) == 0);
    assert(getNodesSwapDelta(nodes, solution, dist, 1, 3) == getNodesSwapDelta(nodes, solution, dist, 3, 1));
    assert(getNodesSwapDelta(nodes, solution, dist, 0, 3) == 2);
    assert(getNodesSwapDelta(nodes, solution, dist, 2, 3) == 1);
    assert(getNodesSwapDelta(nodes, solution, dist, 0, 4) == 2);
    assert(getNodesSwapDelta(nodes, solution, dist, 1, 4) == 1);
    solution.push_back(5);
    assert(getNodesSwapDelta(nodes, solution, dist, 1, 4) == 4);

    int original_evaluation = evaluateSolution(nodes, solution);
    int delta = getNodesSwapDelta(nodes, solution, dist, 0, 5);
    swapNodes(solution, 0, 5);
    int new_evaluation = evaluateSolution(nodes, solution);
    assert(new_evaluation - original_evaluation == delta);

    original_evaluation = new_evaluation;
    delta = getNodesSwapDelta(nodes, solution, dist, 1, 4);
    swapNodes(solution, 1, 4);
    new_evaluation = evaluateSolution(nodes, solution);
    assert(new_evaluation - original_evaluation == delta);
}

void testGetDeltaSwapEdges()
{
    // Note: operations are in-place
    Solution solution = {0, 1, 2, 3, 4, 5};

    assert(getEdgesSwapDelta(nodes, solution, dist, 1, 1) == 0);
    assert(getEdgesSwapDelta(nodes, solution, dist, 1, 3) == getEdgesSwapDelta(nodes, solution, dist, 3, 1));

    assert(getEdgesSwapDelta(nodes, solution, dist, 0, 3) == 2);
    assert(getEdgesSwapDelta(nodes, solution, dist, 2, 3) == 0);
    assert(getEdgesSwapDelta(nodes, solution, dist, 0, 5) == 0);
    assert(getEdgesSwapDelta(nodes, solution, dist, 1, 4) == -1);

    int original_evaluation = evaluateSolution(nodes, solution);
    int delta = getEdgesSwapDelta(nodes, solution, dist, 0, 5);
    swapEdges(solution, 0, 5);
    int new_evaluation = evaluateSolution(nodes, solution);
    assert(new_evaluation - original_evaluation == delta);

    original_evaluation = new_evaluation;
    delta = getEdgesSwapDelta(nodes, solution, dist, 1, 4);
    swapEdges(solution, 1, 4);
    new_evaluation = evaluateSolution(nodes, solution);
    assert(new_evaluation - original_evaluation == delta);
}

int main()
{
    testGetDeltaReplaceNode();
    testGetDeltaSwapNodes();
    testGetDeltaSwapEdges();
}