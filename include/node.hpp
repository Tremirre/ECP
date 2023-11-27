#pragma once
#include <vector>
#include <string>

class Node
{
public:
    Node(int x, int y, int weight);
    int getX() const noexcept;
    int getY() const noexcept;
    int getWeight() const noexcept;

    int distanceTo(const Node &other) const noexcept;

private:
    int m_x;
    int m_y;
    int m_weight;
};

typedef std::vector<Node> Nodes;
typedef std::vector<std::vector<int>> DistanceMatrix;

Nodes importNodesFromFile(const std::string &filename);

DistanceMatrix calculateDistanceMatrix(const Nodes &nodes);

struct NodesDistPair
{
    Nodes nodes;
    DistanceMatrix dist;

    NodesDistPair(Nodes nodes) : nodes(std::move(nodes)), dist(calculateDistanceMatrix(this->nodes)) {}
};
