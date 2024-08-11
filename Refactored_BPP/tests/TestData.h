#ifndef TESTDATA_H
#define TESTDATA_H

#include <vector>
#include <tuple>

struct TestData {
    std::string name;
    std::vector<int> items;
    int binCapacity;
    int expectedBins;
};

const std::vector<TestData> testCases = {
    {"Minimal Input Size - Zero Items", {}, 10, 0},
    {"Minimal Input Size - One Item", {5}, 10, 1},
    {"Maximal Input Size", std::vector<int>(1000, 1), 10, 100},
    {"Skewed Item Sizes", {1, 1, 1, 1, 1, 9}, 10, 2},
    {"Uniform Item Sizes", {5, 5, 5, 5, 5, 5}, 10, 3},
    {"High Packing Density", {8, 7, 6, 5, 4, 3, 2, 1}, 10, 4},
    {"Low Packing Density", {1, 1, 1, 1, 1, 1}, 10, 1},
    {"Boundary Conditions", {10, 10, 10, 10}, 10, 4}
};

#endif // TESTDATA_H
