#pragma once
#include <string>
#include <vector>

class ArgParser
{
public:
    ArgParser(int &argc, char **argv);
    const std::string &getCmdOption(const std::string &option) const;
    const bool cmdOptionExists(const std::string &option) const;

private:
    std::vector<std::string> m_tokens;
};