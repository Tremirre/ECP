#include <cassert>
#include <iostream>
#include <fstream>
#include "solution.hpp"

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
    exportSolutionToFile(solution, "test_solution.txt", score);

    std::ifstream test_file("test_solution.txt");

    assert(test_file.is_open());

    std::string line;
    int i = 0;
    while (std::getline(test_file, line))
    {
        int expected = i == 5 ? score : solution[i++];
        assert(std::stoi(line) == expected);
    }
    test_file.close();

    std::remove("test_solution.txt");
}

void testImportSolutionFromFile()
{
    std::ofstream test_file("test_solution.txt", std::ios::out | std::ios::trunc);
    test_file << "0\n1\n2\n3\n4\n100\n";
    test_file.close();

    Solution solution = importSolutionFromFile("test_solution.txt");
    assert(solution.size() == 5);
    assert(solution[0] == 0);
    assert(solution[1] == 1);
    assert(solution[2] == 2);
    assert(solution[3] == 3);
    assert(solution[4] == 4);

    std::remove("test_solution.txt");
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

void testReplaceNode()
{
    // Note: operations are in-place
    Solution solution = {0, 1, 2, 3, 4};
    Solution expected_1 = {0, 1, 2, 5, 4};
    Solution expected_2 = {0, 1, 2, 5, 6};
    Solution expected_3 = {7, 1, 2, 5, 6};

    assert(replaceNode(solution, 3, 5) == expected_1);
    assert(replaceNode(solution, 4, 6) == expected_2);
    assert(replaceNode(solution, 0, 7) == expected_3);
}

void testSwapNodes()
{
    // Note: operations are in-place
    Solution solution = {0, 1, 2, 3, 4};
    Solution expected_1 = {0, 1, 2, 3, 4};
    Solution expected_2 = {0, 1, 4, 3, 2};
    Solution expected_3 = {0, 2, 4, 3, 1};

    assert(swapNodes(solution, 0, 0) == expected_1);
    assert(swapNodes(solution, 2, 4) == expected_2);
    assert(swapNodes(solution, 1, 4) == expected_3);
}

void testSwapEdges()
{
    // Note: operations are in-place
    Solution solution = {0, 1, 2, 3, 4, 5};
    Solution expected_1 = {0, 1, 2, 3, 4, 5};
    Solution expected_2 = {0, 1, 2, 4, 3, 5};
    Solution expected_3 = {0, 3, 4, 2, 1, 5};

    assert(swapEdges(solution, 3, 3) == expected_1);
    assert(swapEdges(solution, 0, 5) == expected_1);
    assert(swapEdges(solution, 3, 4) == expected_1);
    assert(swapEdges(solution, 2, 4) == expected_2);
    assert(swapEdges(solution, 0, 4) == expected_3);

    Solution s1 = {0, 1, 2, 3, 4, 5};
    Solution s2 = {0, 1, 2, 3, 4, 5};
    assert(swapEdges(s1, 1, 3) == swapEdges(s2, 3, 1));
}

int main()
{
    testExportSolutionToFile();
    testImportSolutionFromFile();
    testEvaulateSolution();

    testReplaceNode();
    testSwapNodes();
    testSwapEdges();
}