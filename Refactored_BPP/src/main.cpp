#include "BinPackingGame.h"
#include "AVLTree.h"
#include "MTP.cpp"
#include <vector>
#include <string>
#include <iostream>
#include <chrono>
#include <sstream>
#include <algorithm>
#include <set>

using namespace std;

bool validatePacking(const std::vector<Bin>& bins, const std::vector<int>& items) {
    std::vector<bool> itemUsed(items.size(), false);

    // Check if all items are used exactly once and bins do not exceed their capacity
    for (const auto& bin : bins) {
        int binTotal = bin.sumContents();
        if (binTotal > bin.capacity) {
            std::cerr << "Bin exceeds capacity with total " << binTotal << "!" << std::endl;
            return false;
        }

        const std::vector<int>& binItems = bin.contents;
        for (int item : binItems) {
            // Check if the item exists in the original list
            auto it = std::find(items.begin(), items.end(), item);
            if (it == items.end()) {
                std::cerr << "Item " << item << " not found in original list!" << std::endl;
                return false;
            }

            // Check if the item has already been used
            int index = std::distance(items.begin(), it);
            if (itemUsed[index]) {
                std::cerr << "Item " << item << " used multiple times!" << std::endl;
                return false;
            }

            itemUsed[index] = true;
        }
    }

    // Ensure all items are used
    for (size_t i = 0; i < items.size(); ++i) {
        if (!itemUsed[i]) {
            std::cerr << "Item " << items[i] << " not used in any bin!" << std::endl;
            return false;
        }
    }

    std::cout << "Packing is valid!" << std::endl;
    return true;
}

int generatePerftTables() {
    // Example bin-packing instance
    int bc = 150;
    std::vector<int> items = {42,69,67,57,93,90,38};

    BinPackingGame game(items,bc);

    // Set up the game with a small bin-packing instance
    int maxDepth = 15; // Set a reasonable depth for the perft table

    std::cout << "Running Minimax Alpha-Beta Pruning...\n";
    game.minimaxAlphaBeta(0, maxDepth, INT_MIN, INT_MAX);

    // Output the perft table
    std::cout << "Perft Table (Alpha-Beta):" << std::endl;
    for (int depth = 0; depth < game.perftTable.size(); ++depth) {
        std::cout << "Depth " << depth << ": " << game.perftTable[depth] << " nodes" << std::endl;
    }

    std::cout << "\nRunning Pure Minimax...\n";
    game.perftTable.clear();  // Reset perft table
    game.pureMinimax(0, maxDepth);
    // Output the perft table for pure minimax
    std::cout << "Perft Table (Pure Minimax):" << std::endl;
    for (int depth = 0; depth < game.perftTable.size(); ++depth) {
        std::cout << "Depth " << depth << ": " << game.perftTable[depth] << " nodes" << std::endl;
    }

    return 0;
}



void printResults(const string &algorithm, int binsUsed, double runtime, int itemsCount) {
    cout << "{\"Algorithm\":\"" << algorithm
         << "\", \"BinsUsed\":" << binsUsed
         << ", \"Runtime\":" << runtime
         << ", \"ItemsCount\":" << itemsCount << "}" << endl;
}


int main(int argc, char *argv[]) {
    if (argc < 7) {
        cerr << "Usage: " << argv[0]
             << " <bin_capacity> <best_known_bins> <item_sizes> <algorithms> <data_structure> <printConfiguration (Y/N)>\n";
        return 1;
    }

    int binCapacity = stoi(argv[1]);
    int bestKnownBins = stoi(argv[2]);
    vector<int> items;
    stringstream ss(argv[3]);
    string item;
    while (getline(ss, item, ',')) {
        items.push_back(stoi(item));
    }

    sort(items.begin(), items.end(), greater<int>());

    string algorithmsStr = argv[4];
    vector<string> algorithms;
    stringstream algStream(algorithmsStr);
    string alg;
    while (getline(algStream, alg, ',')) {
        algorithms.push_back(alg);
    }

    string dataStructure = argv[5];
    if(dataStructure == "OOP"){
        for (const string &algorithm : algorithms) {
            auto start = chrono::high_resolution_clock::now();
            BinPackingGame game(items, binCapacity);

            if (algorithm == "BranchAndBound") {
                auto end = chrono::high_resolution_clock::now();
                printResults("Branch and Bound", game.getBestBins().size(),
                            chrono::duration<double>(end - start).count(), items.size());
            } else if (algorithm == "BestFit") {
                game.bestFit();
                auto end = chrono::high_resolution_clock::now();
                printResults("Best Fit", game.getBestBins().size(),
                            chrono::duration<double>(end - start).count(), items.size());
            } else if (algorithm == "FirstFit") {
                game.firstFit();
                auto end = chrono::high_resolution_clock::now();
                printResults("First Fit", game.getBestBins().size(),
                            chrono::duration<double>(end - start).count(), items.size());
            } else if (algorithm == "NextFit") {
                game.nextFit();
                auto end = chrono::high_resolution_clock::now();
                printResults("Next Fit", game.getBestBins().size(),
                            chrono::duration<double>(end - start).count(), items.size());
            } else if (algorithm == "WorstFit") {
                game.worstFit();
                auto end = chrono::high_resolution_clock::now();
                printResults("Worst Fit", game.getBestBins().size(),
                            chrono::duration<double>(end - start).count(), items.size());
            } else if (algorithm == "BestFitDecreasing") {
                game.bestFitDecreasing();
                auto end = chrono::high_resolution_clock::now();
                printResults("Best Fit Decreasing", game.getBestBins().size(),
                            chrono::duration<double>(end - start).count(), items.size());
            } else if (algorithm == "FirstFitDecreasing") {
                game.firstFitDecreasing();
                auto end = chrono::high_resolution_clock::now();
                printResults("First Fit Decreasing", game.getBestBins().size(),
                            chrono::duration<double>(end - start).count(), items.size());
            } else if (algorithm == "NextFitDecreasing") {
                game.nextFitDecreasing();
                auto end = chrono::high_resolution_clock::now();
                printResults("Next Fit Decreasing", game.getBestBins().size(),
                            chrono::duration<double>(end - start).count(), items.size());
            } else if (algorithm == "WorstFitDecreasing") {
                game.worstFitDecreasing();
                auto end = chrono::high_resolution_clock::now();
                printResults("Worst Fit Decreasing", game.getBestBins().size(),
                            chrono::duration<double>(end - start).count(), items.size());
            } else if (algorithm == "Hybrid") {
                int maxDepth = items.size(); // Adjust depth as required for your algorithm
                int bestValue = game.iterativeDeepeningMinimax(maxDepth,0.2);
                auto end = chrono::high_resolution_clock::now();
                printResults("Hybrid", game.getBestBins().size(),
                            chrono::duration<double>(end - start).count(), items.size());
            }else {
                cerr << "Unknown algorithm: " << algorithm << endl;
                return -1;
            }
        }
    }
    else if(dataStructure == "AVLTree"){
        AVLTree avlTree;
        
        for (const string &algorithm : algorithms) {
            if(algorithm == "FirstFitDecreasing"){
                auto start = chrono::high_resolution_clock::now();
                int binCount = avlTree.firstFitDecreasing(items, binCapacity);
                auto end = chrono::high_resolution_clock::now();
                printResults("First Fit Decreasing", binCount,
                            chrono::duration<double>(end - start).count(), items.size());
            } else if(algorithm == "BestFitDecreasing"){
                auto start = chrono::high_resolution_clock::now();
                int binCount = avlTree.bestFitDecreasing(items, binCapacity);
                auto end = chrono::high_resolution_clock::now();
                printResults("Best Fit Decreasing", binCount,
                            chrono::duration<double>(end - start).count(), items.size());
            } else if(algorithm == "WorstFitDecreasing"){
                auto start = chrono::high_resolution_clock::now();
                int binCount = avlTree.worstFitDecreasing(items, binCapacity);
                auto end = chrono::high_resolution_clock::now();
                printResults("Worst Fit Decreasing", binCount,
                            chrono::duration<double>(end - start).count(), items.size());
            } else if(algorithm == "NextFitDecreasing"){
                auto start = chrono::high_resolution_clock::now();
                int binCount = avlTree.nextFitDecreasing(items, binCapacity);
                auto end = chrono::high_resolution_clock::now();
                printResults("Next Fit Decreasing", binCount,
                            chrono::duration<double>(end - start).count(), items.size());
            } else {
                cerr << "Unknown algorithm: " << algorithm << endl;
                return -1;
            }
        }
    }
    else if(dataStructure == "MTP"){
        auto start = chrono::high_resolution_clock::now();
        for(const string &algorithm : algorithms){
            if (algorithm == "MTP"){
                    std::sort(items.begin(), items.end(), std::greater<>());
                    int n = items.size();
                    int jdim = n;
                    int back = -1;
                    int jck = 1;
                    int z;
                    std::vector<int> xstar(jdim, 0);
                    int lb;
                    mtp(n, items, binCapacity, jdim, back, jck, z, xstar, lb);
                    auto end = chrono::high_resolution_clock::now();
                    printResults("MTP", z, chrono::duration<double>(end - start).count(), n);
            }
            else{
                cerr << "Invalid Algorithm" << endl;
            }
        }
    }
    else{
        cerr << "Invalid Data Structure" << endl;
    }

    return 0;
}
