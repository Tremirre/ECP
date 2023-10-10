#include <iostream>
#include <string>

#include "solvers.hpp"
#include "node.hpp"
#include "argparser.hpp"

int main(int argc, char **argv)
{
    ArgParser args(argc, argv);

    std::string filename = args.getCmdOption("-f");
    if (filename.empty())
    {
        std::cout << "No input file specified" << std::endl;
        return 1;
    }

    std::string output_filename = args.getCmdOption("-o");
    output_filename = output_filename.empty() ? "solution.txt" : output_filename;

    Nodes nodes = readNodesFromFile(filename);

    std::string start_idx_str = args.getCmdOption("-i");
    start_idx_str = start_idx_str.empty() ? "0" : start_idx_str;
    int start_idx = std::stoi(start_idx_str);
    if (start_idx < 0 || start_idx >= nodes.size())
    {
        std::cout << "Invalid start index" << std::endl;
        return 1;
    }

    std::string start_perc_str = args.getCmdOption("-p");
    start_perc_str = start_perc_str.empty() ? "100" : start_perc_str;
    double start_perc = std::stoi(start_perc_str) / 100.0;
    int visit_count = start_perc * nodes.size();

    if (visit_count < 0 || visit_count > nodes.size())
    {
        std::cout << "Invalid start perc count" << std::endl;
        return 1;
    }

    char solver_name = args.getCmdOption("-s")[0];
    AbstractSolver *solver = createSolver(solver_name);

    Solution solution = solver->solve(nodes, start_idx, visit_count);
    exportSolutionToFile(solution, output_filename);
    std::cout << "Used nodes: " << solution.size() << " / " << nodes.size() << std::endl;
    std::cout << "Score: " << evaluateSolution(nodes, solution) << std::endl;
    std::cout << "Solution exported to " << output_filename << std::endl;

    delete solver;
    return 0;
}
