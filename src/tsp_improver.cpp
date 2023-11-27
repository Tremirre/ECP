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

    std::string sub_type = args.getCmdOption("-st");
    sub_type = sub_type.empty() ? "p" : sub_type;

    std::string ntype = args.getCmdOption("-n");
    ntype = ntype.empty() ? "both" : ntype;

    std::string param = args.getCmdOption("-p");
    param = param.empty() ? "0" : param;
    int param_int = std::stoi(param);

    std::string subparam_1 = args.getCmdOption("-sp1");
    subparam_1 = subparam_1.empty() ? "10" : subparam_1;
    double subparam_1_double = std::stod(subparam_1);

    std::string subparam_2 = args.getCmdOption("-sp2");
    subparam_2 = subparam_2.empty() ? "10" : subparam_2;
    int subparam_2_int = std::stod(subparam_2);

    NodesDistPair nodes{importNodesFromFile(instance_filename)};
    Solution solution = importSolutionFromFile(solution_filename);
    auto resolved_ntype = getNeighborhoodType(ntype);
    auto improver = createImprover(improver_type[0], resolved_ntype, param_int, sub_type[0], subparam_1_double, subparam_2_int);
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