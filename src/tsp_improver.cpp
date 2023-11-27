#include <iostream>
#include <string>
#include <chrono>

#include "improvers.hpp"
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

    std::string improver_type = args.getCmdOption("-t");
    if (improver_type.empty())
    {
        std::cout << "No improver type specified" << std::endl;
        return 1;
    }

    std::string ntype = args.getCmdOption("-n");
    ntype = ntype.empty() ? "both" : ntype;

    std::string param = args.getCmdOption("-p");
    param = param.empty() ? "0" : param;
    int param_int = std::stoi(param);

    NodesDistPair nodes{importNodesFromFile(instance_filename)};
    Solution solution = importSolutionFromFile(solution_filename);
    auto resolved_ntype = getNeighborhoodType(ntype);
    auto improver = createImprover(improver_type[0], resolved_ntype, param_int);
    const auto start = std::chrono::high_resolution_clock::now();
    solution = improver->improve(solution, nodes);
    const auto end = std::chrono::high_resolution_clock::now();

    const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    int microseconds = elapsed.count();
    int score = evaluateSolution(nodes.nodes, solution);
    exportSolutionToFile(
        solution,
        output_filename,
        score,
        microseconds);
    std::cout << "Score: " << score << std::endl;
    std::cout << "Time: " << elapsed.count() << " us\n";
    std::cout << "Solution exported to " << output_filename << std::endl;
    return 0;
}