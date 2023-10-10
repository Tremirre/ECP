#include "node.hpp"
#include <cmath>
#include <fstream>
#include <sstream>

Node::Node(int x, int y, int weight)
    : m_x(x), m_y(y), m_weight(weight)
{
}

int Node::getX() const noexcept
{
    return m_x;
}

int Node::getY() const noexcept
{
    return m_y;
}

int Node::getWeight() const noexcept
{
    return m_weight;
}

int Node::distanceTo(const Node &other) const noexcept
{
    int dx = m_x - other.getX();
    int dy = m_y - other.getY();
    return std::sqrt(dx * dx + dy * dy);
}

Nodes readNodesFromFile(const std::string &filename)
{
    Nodes nodes;
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file");
    }
    std::string line;
    std::string token;
    std::istringstream iss("");
    int x, y, weight;
    while (std::getline(file, line))
    {
        iss.clear();
        iss.str(line);
        if (std::getline(iss, token, ';'))
        {
            x = std::stoi(token);
        }
        if (std::getline(iss, token, ';'))
        {
            y = std::stoi(token);
        }
        if (std::getline(iss, token, ';'))
        {
            weight = std::stoi(token);
        }
        Node node(x, y, weight);
        nodes.push_back(node);
    }
    return nodes;
}