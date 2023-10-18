#include <cassert>
#include <iostream>
#include <fstream>
#include "node.hpp"
#include "solvers.hpp"

static const Nodes cases_1 = {
    {0, 0, 1},
    {10, 0, 2},
    {5, 5, 0},
};

static const Nodes cases_2 = {
    {0, 0, 1},
    {10, 0, 2},
    {0, 10, 3},
    {10, 10, 4},
    {30, 20, 4},
    {-10, 4, 4},
};

static const Nodes cases_3 = {
    {0, 15, 0},
    {0, 23, -3},
    {12, 4, 50},
    {32, 2, 4},
    {0, 10, -20},
    {14, 6, -45},
    {10, 15, -2},
    {0, -10, 20},
    {10, 2, 50},
};

void testExportSolutionToFile()
{
    Solution solution = {0, 1, 2, 3, 4};
    int score = 100;
    exportSolutionToFile(solution, "test_solution.csv", score);

    std::ifstream test_file("test_solution.csv");

    assert(test_file.is_open());

    std::string line;
    int i = 0;
    while (std::getline(test_file, line))
    {
        int expected = i == 5 ? score : solution[i++];
        assert(std::stoi(line) == expected);
    }
    test_file.close();

    std::remove("test_solution.csv");
}

void testEvaulateSolution()
{
    Solution solution_1 = {1, 0, 2};
    Solution solution_2 = {4, 3, 0, 1, 5, 2};
    Solution solution_3 = {8, 2, 5, 0};

    assert(evaluateSolution(cases_1, solution_1) == 27);
    assert(evaluateSolution(cases_2, solution_2) == 128);
    assert(evaluateSolution(cases_3, solution_3) == 94);
}

int main()
{
    testExportSolutionToFile();
    testEvaulateSolution();
}