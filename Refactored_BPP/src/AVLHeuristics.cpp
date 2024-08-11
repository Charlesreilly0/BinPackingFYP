#include "AVLTree.h"
#include <iostream>
#include <vector>

int main() {
    AVLTree avlTree;

    // Example items and bin capacity
    int binCapacity = 10;
    std::vector<int> items = {1,1,1,1,1,1,1,1};

    std::cout << "Items: ";
    for (int item : items) {
        std::cout << item << " ";
    }
    std::cout << std::endl;

    std::cout << "\nUsing First Fit Decreasing Heuristic:" << std::endl;
    int binCount = avlTree.firstFitDecreasing(items, binCapacity);
    std::cout << "Number of bins used: " << binCount << std::endl;

    // Clear the tree for the next test
    avlTree.clear();

    std::cout << "\nUsing Best Fit Decreasing Heuristic:" << std::endl;
    binCount = avlTree.bestFitDecreasing(items, binCapacity);
    std::cout << "Number of bins used: " << binCount << std::endl;

    // Clear the tree for the next test
    avlTree.clear();

    std::cout << "\nUsing Worst Fit Decreasing Heuristic:" << std::endl;
    binCount = avlTree.worstFitDecreasing(items, binCapacity);
    std::cout << "Number of bins used: " << binCount << std::endl;

    // Clear the tree for the next test
    avlTree.clear();

    std::cout << "\nUsing Next Fit Decreasing Heuristic:" << std::endl;
    binCount = avlTree.nextFitDecreasing(items, binCapacity);
    std::cout << "Number of bins used: " << binCount << std::endl;

    return 0;
}
