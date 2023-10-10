#include "argparser.hpp"
#include <algorithm>

ArgParser::ArgParser(int &argc, char **argv)
{
    for (int i = 1; i < argc; ++i)
    {
        m_tokens.push_back(std::string(argv[i]));
    }
};

const std::string &ArgParser::getCmdOption(const std::string &option) const
{
    std::vector<std::string>::const_iterator itr;
    itr = std::find(m_tokens.begin(), m_tokens.end(), option);
    if (itr != m_tokens.end() && ++itr != m_tokens.end())
    {
        return *itr;
    }
    static const std::string empty_string("");
    return empty_string;
};

const bool ArgParser::cmdOptionExists(const std::string &option) const
{
    return std::find(m_tokens.begin(), m_tokens.end(), option) != m_tokens.end();
};