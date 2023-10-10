#include <cassert>
#include <iostream>
#include <fstream>
#include "node.hpp"

void testNodeDistancesCalculatedCorrectly()
{
    Node nodes[] = {
        {0, 0, 1},
        {10, 0, 2},
        {5, 5, 0}};
    int expected_distances[3][3] = {
        {0, 10, 7},
        {10, 0, 7},
        {7, 7, 0}};

    for (std::size_t i = 0; i < 3; ++i)
    {
        for (std::size_t j = 0; j < 3; ++j)
        {
            int distance = nodes[i].distanceTo(nodes[j]);
            assert(distance == expected_distances[i][j]);
        }
    };
}

void testNodesReadCorrectlyFromFile()
{
    std::ofstream test_file("test_nodes.csv", std::ios::out | std::ios::trunc);
    test_file << "0;0;1\n10;0;2\n5;5;0\n";
    test_file.close();

    Nodes nodes = readNodesFromFile("test_nodes.csv");
    assert(nodes.size() == 3);
    assert(nodes[0].getX() == 0);
    assert(nodes[0].getY() == 0);
    assert(nodes[0].getWeight() == 1);
    assert(nodes[1].getX() == 10);
    assert(nodes[1].getY() == 0);
    assert(nodes[1].getWeight() == 2);
    assert(nodes[2].getX() == 5);
    assert(nodes[2].getY() == 5);
    assert(nodes[2].getWeight() == 0);

    std::remove("test_nodes.csv");
}

int main()
{
    testNodeDistancesCalculatedCorrectly();
    testNodesReadCorrectlyFromFile();
}