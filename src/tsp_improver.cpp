#include <iostream>
#include <string>

#include "solvers.hpp"
#include "node.hpp"
#include "argparser.hpp"

int main(int argc, char **argv)
{
    ArgParser args(argc, argv);

    std::string instance_filename = args.getCmdOption("-f");
    if (instance_filename.empty())
    {
        std::cout << "No input file specified" << std::endl;
        return 1;
    }

    std::string solution_filename = args.getCmdOption("-s");
    if (solution_filename.empty())
    {
        std::cout << "No solution file specified" << std::endl;
        return 1;
    }

    std::string output_filename = args.getCmdOption("-o");
    output_filename = output_filename.empty() ? "solution.txt" : output_filename;

    Nodes nodes = importNodesFromFile(instance_filename);
    Solution solution = importSolutionFromFile(solution_filename);

    std::cout << "Score: " << evaluateSolution(nodes, solution) << std::endl;

    return 0;
}