#include <vector>
#include <limits>
#include <algorithm>
#include <iostream>
#include <unordered_map>

// Bin class to manage items in bins
class Bin {
public:
    int capacity;
    int load;
    std::vector<int> items;

    Bin(int cap) : capacity(cap), load(0) {}

    bool canAdd(int item) {
        return (load + item) <= capacity;
    }

    void addItem(int item) {
        load += item;
        items.push_back(item);
    }

    void removeLastItem() {
        load -= items.back();
        items.pop_back();
    }
};

// Global variables
std::vector<Bin> bins; // Vector representing current bins
std::vector<int> items = {36,26,37,46,26,27,40,26,33,37,30,32,46,25,28,39,26,33,49,25,25,46,26,27,49,25,25,41,26,31,38,27,34,36,29,33,46,25,27,48,25,25,48,25,25,44,27,28,35,28,36,42,26,31,46,26,27,44,26,29,35,26,38,44,26,29,44,25,29,36,31,32,49,25,25,39,28,32,47,25,27,46,25,27,42,26,30,45,26,27,40,25,34,42,25,31,39,25,35,44,27,27,49,25,25,46,26,26,49,25,25,46,26,27,49,25,25,42,25,31,49,25,25,45,26,27,48,25,26,47,25,26,42,28,28,41,25,33,39,26,34,39,26,33,45,26,27,43,26,30,38,28,32,43,26,29,48,25,26,47,26,26,47,25,26,37,27,35,38,30,30,43,28,28,38,28,32,37,31,31,45,26,28,46,26,27,41,27,31,41,26,32,40,25,34,47,25,27,38,28,33,42,27,30,40,27,32,36,28,35,46,26,27,48,25,26,39,28,32,48,25,26,49,25,25,48,25,26,37,26,36,46,25,28,40,25,34,35,27,37,47,25,26,43,25,31,41,25,32,42,26,30,35,30,33,46,25,27,49,25,25,39,29,30,47,25,27,35,31,32,45,27,27,43,26,29,46,25,27,35,29,35,43,25,30,47,25,27,49,25,25,35,29,34,39,28,31,43,25,31,42,27,30,44,25,30,49,25,25,37,27,34,42,26,31,36,25,38,40,27,31,41,25,33,41,26,31,45,25,29,44,25,29,37,29,33,41,28,30,35,27,37,37,29,33,38,25,35,48,25,25,39,29,30,43,26,30,48,25,26,48,25,26,35,26,37,45,25,29,36,26,37,41,28,30,41,25,33,40,28,31,48,25,26,36,31,31,47,25,26,44,25,30,47,26,26,37,27,34,48,25,26,37,29,32,37,26,35,42,25,32,47,25,26,49,25,25,48,25,25,46,25,28,49,25,25,43,27,28,47,25,26,40,28,30,45,25,29,42,25,31,47,25,26,48,25,26,41,27,31,41,25,33,44,25,29,48,25,26,39,26,34,40,28,31,42,25,32,35,28,36,49,25,25,41,29,29,35,30,34,38,27,33,47,25,26,48,25,26,48,25,26,46,25,27,42,26,31,41,25,34 }; // Items to be packed
std::vector<bool> placedItems(items.size(), false); // Track placed items
int binCapacity = 100; // Example bin capacity

std::unordered_map<size_t, int> stateCache; // Cache for storing evaluated states

size_t hashState() {
    size_t seed = bins.size();
    for (const auto& bin : bins) {
        seed ^= bin.load + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        for (int item : bin.items) {
            seed ^= item + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
    }
    for (bool placed : placedItems) {
        seed ^= placed + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
}

void printConfiguration() {
    std::cout << "Configuration with " << bins.size() << " bins:" << std::endl;
    for (size_t i = 0; i < bins.size(); ++i) {
        std::cout << "Bin " << i + 1 << ": ";
        for (int item : bins[i].items) {
            std::cout << item << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "-----" << std::endl;
}

int finalabbo(int depth, int maxDepth, int alpha, int beta) {
    size_t stateHash = hashState();
    if (stateCache.find(stateHash) != stateCache.end()) {
        return stateCache[stateHash];
    }

    int currentEval = bins.size();
    if (std::all_of(placedItems.begin(), placedItems.end(), [](bool v) { return v; })) {
        std::cout << "All items placed and packing validated. Current Eval: " << currentEval << std::endl;
        printConfiguration();
        stateCache[stateHash] = currentEval;
        return currentEval;
    }

    int maxValue = std::numeric_limits<int>::min();
    int minValue = std::numeric_limits<int>::max();

    std::vector<std::pair<int, int>> moves;
    for (size_t i = 0; i < items.size(); ++i) {
        if (!placedItems[i]) {
            for (size_t j = 0; j < bins.size(); ++j) {
                if (bins[j].canAdd(items[i])) {
                    moves.emplace_back(i, j);
                }
            }
            moves.emplace_back(i, bins.size());
        }
    }

    for (const auto& move : moves) {
        int itemIndex = move.first;
        int binIndex = move.second;

        std::cout << "Trying move: Item " << itemIndex << " -> Bin " << binIndex << std::endl;

        if (binIndex < bins.size()) {
            bins[binIndex].addItem(items[itemIndex]);
            placedItems[itemIndex] = true;

            std::cout << "Placed item " << itemIndex << " in existing bin " << binIndex << std::endl;

            int newValue = finalabbo(depth + 1, maxDepth, alpha, beta);

            bins[binIndex].removeLastItem();
            placedItems[itemIndex] = false;

            maxValue = std::max(maxValue, newValue);
            minValue = std::min(minValue, newValue);

            alpha = std::max(alpha, maxValue);
            beta = std::min(beta, minValue);

        } else {
            Bin newBin(binCapacity);
            newBin.addItem(items[itemIndex]);
            bins.push_back(newBin);
            placedItems[itemIndex] = true;

            std::cout << "Placed item " << itemIndex << " in a new bin " << bins.size() - 1 << std::endl;

            int newValue = finalabbo(depth + 1, maxDepth, alpha, beta);

            bins.pop_back();
            placedItems[itemIndex] = false;

            maxValue = std::max(maxValue, newValue);
            minValue = std::min(minValue, newValue);

            alpha = std::max(alpha, maxValue);
            beta = std::min(beta, minValue);

        }

        if (beta <= alpha) {
            break;
        }
    }

    int finalValue = std::max(maxValue, minValue);
    stateCache[stateHash] = finalValue;
    std::cout << "Depth: " << depth << " | Returning final value: " << finalValue << std::endl;
    return finalValue;
}

int main() {
    int result = finalabbo(0, 10, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    std::cout << "Minimum number of bins used: " << result << std::endl;
    return 0;
}
