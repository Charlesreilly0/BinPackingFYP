#include "BinPackingGame.h"
#include <set>
#include <unordered_set>


size_t BinPackingGame::hashState() {
    size_t hash = 0;
    const size_t prime = 0x9e3779b9; // A large prime number for hashing

    for (size_t binIndex = 0; binIndex < bins.size(); ++binIndex) {
        const Bin& bin = bins[binIndex];
        for (size_t itemIndex = 0; itemIndex < bin.contents.size(); ++itemIndex) {
            int item = bin.contents[itemIndex];
            // Combine item value with its index in a unique way
            hash ^= std::hash<int>()(item) + prime + (hash << 6) + (hash >> 2);
            hash ^= std::hash<size_t>()(itemIndex) + prime + (hash << 6) + (hash >> 2);
        }
        // Include the bin's residual capacity in the hash
        hash ^= std::hash<int>()(bin.getResidualCapacity()) + prime + (hash << 6) + (hash >> 2);
    }

    // Combine the overall structure into the final hash
    hash ^= std::hash<size_t>()(bins.size()) + prime + (hash << 6) + (hash >> 2);
    return hash;
}


void BinPackingGame::preprocessPerfectFits(double proportion) {
    size_t perfectFitLimit = static_cast<size_t>(proportion * items.size());
    size_t perfectFitCount = 0;

    for (size_t i = 0; i < items.size() && perfectFitCount < perfectFitLimit;
         ++i) {
      if (!placedItems[i]) {
        bool perfectFitFound = false;

        // Attempt to find a perfect fit in existing bins
        for (auto &bin : bins) {
          if (bin.canAdd(items[i]) && bin.getResidualCapacity() == items[i]) {
            bin.addItem(items[i]);
            placedItems[i] = true;
            perfectFitFound = true;
            perfectFitCount++;
            break;
          }
        }

        // Stop looking for perfect fits if we've reached the limit
        if (perfectFitFound && perfectFitCount >= perfectFitLimit) {
          continue;
        }

        // If no perfect fit is found, place the item in the best bin or create
        // a new bin
        if (!perfectFitFound) {
          int bestBinIndex = -1;
          int minResidualCapacity = INT_MAX;
          for (size_t j = 0; j < bins.size(); ++j) {
            if (bins[j].canAdd(items[i]) &&
                bins[j].getResidualCapacity() < minResidualCapacity) {
              bestBinIndex = j;
              minResidualCapacity = bins[j].getResidualCapacity();
            }
          }
          if (bestBinIndex != -1) {
            bins[bestBinIndex].addItem(items[i]);
            placedItems[i] = true;
          } else {
            // If no existing bin can fit, create a new bin
            Bin newBin(binCapacity);
            newBin.addItem(items[i]);
            bins.push_back(newBin);
            placedItems[i] = true;
          }
        }
      }
    }
  }

  void BinPackingGame::preprocessTriplets(double proportion) {
    size_t perfectFitLimit = static_cast<size_t>(proportion * items.size());
    size_t perfectFitCount = 0;

    for (size_t i = 0; i < items.size() && perfectFitCount < perfectFitLimit; ++i) {
        if (!placedItems[i]) {
            bool perfectFitFound = false;

            for (size_t j = i + 1; j < items.size(); ++j) {
                if (!placedItems[j]) {
                    for (size_t k = j + 1; k < items.size(); ++k) {
                        if (!placedItems[k]) {
                            int sum = items[i] + items[j] + items[k];
                            if (sum <= binCapacity) {
                                // Simulate the placement to see the impact
                                int residualAfterPlacement = binCapacity - sum;
                                int futureImpact = evaluateFutureImpact(residualAfterPlacement, i, j, k);

                                if (futureImpact <= residualAfterPlacement) {
                                    Bin newBin(binCapacity);
                                    newBin.addItem(items[i]);
                                    newBin.addItem(items[j]);
                                    newBin.addItem(items[k]);
                                    bins.push_back(newBin);

                                    placedItems[i] = true;
                                    placedItems[j] = true;
                                    placedItems[k] = true;

                                    perfectFitCount++;
                                    perfectFitFound = true;
                                    break;
                                }
                            }
                        }
                    }
                    if (perfectFitFound) break;
                }
            }

            if (perfectFitFound && perfectFitCount >= perfectFitLimit) {
                continue;
            }

            if (!perfectFitFound) {
                placeInBestFitOrNewBin(i);
            }
        }
    }
}

int BinPackingGame::evaluateFutureImpact(int residualAfterPlacement, size_t i, size_t j, size_t k) {
    int futureImpact = 0;
    for (size_t x = 0; x < items.size(); ++x) {
        if (x != i && x != j && x != k && !placedItems[x]) {
            futureImpact += std::abs(residualAfterPlacement - items[x]);
        }
    }
    return futureImpact;
}

void BinPackingGame::placeInBestFitOrNewBin(size_t itemIndex) {
    int bestBinIndex = -1;
    int minResidualCapacity = INT_MAX;

    for (size_t j = 0; j < bins.size(); ++j) {
        if (bins[j].canAdd(items[itemIndex]) && bins[j].getResidualCapacity() < minResidualCapacity) {
            bestBinIndex = j;
            minResidualCapacity = bins[j].getResidualCapacity();
        }
    }

    if (bestBinIndex != -1) {
        bins[bestBinIndex].addItem(items[itemIndex]);
        placedItems[itemIndex] = true;
    } else {
        Bin newBin(binCapacity);
        newBin.addItem(items[itemIndex]);
        bins.push_back(newBin);
        placedItems[itemIndex] = true;
    }
}


int BinPackingGame::scoreMove(int itemIndex, int binIndex) const {
    if (binIndex < bins.size()) {
        // Score for placing in an existing bin
        int newResidualCapacity = bins[binIndex].getResidualCapacity() - items[itemIndex];
        if (newResidualCapacity < 0)
            return INT_MIN; // Infeasible move, can't add item

        // Prefer moves that significantly reduce residual capacity without overfilling
        return binCapacity - newResidualCapacity;
    } else {
        // Score for placing in a new bin
        return -binCapacity / 2; // Penalize creating a new bin, but less harshly
    }
}

bool BinPackingGame::validatePacking() {
    bins = bins;
    items = items;
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
                return false;
            }

            // Check if the item has already been used
            int index = std::distance(items.begin(), it);
            if (itemUsed[index]) {
                return false;
            }

            itemUsed[index] = true;
        }
    }

    // Ensure all items are used
    for (size_t i = 0; i < items.size(); ++i) {
        if (!itemUsed[i]) {
            return false;
        }
    }

    return true;
}

void BinPackingGame::updateBestConfigurationBasedOnBins() {
    // Get the current number of bins used
    int currentBinCount = bins.size();

    // Check if the current configuration uses fewer bins than the best configuration so far
    if (currentBinCount < bestBinCount || bestBinCount == 0) {
        std::cout << "Found a better configuration with " << currentBinCount << " bins!" << std::endl;
        bestBinCount = currentBinCount;
        bestBins = bins; // Update the best configuration
    }
}


int BinPackingGame::hybridMethod(int depth, int maxDepth, int alpha, int beta) {
    size_t stateHash = hashState();
    if (stateCache.find(stateHash) != stateCache.end()) {
        return stateCache[stateHash];
    }
    int currentEval_Heuristics = evaluateState(evalParams); // Evaluate the current state
    int currentEval_Bounded = bins.size();
    int currentEval = std::min(currentEval_Heuristics,currentEval_Bounded);
    if (std::all_of(placedItems.begin(), placedItems.end(), [](bool v) { return v; })) {
        updateBestConfiguration(currentEval);
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


        if (binIndex < bins.size()) {
            bins[binIndex].addItem(items[itemIndex]);
            placedItems[itemIndex] = true;


            int newValue = hybridMethod(depth + 1, maxDepth, alpha, beta);
            int nextEval = newValue;
            updateWeights(currentEval, nextEval, evalParams);
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


            int newValue = hybridMethod(depth + 1, maxDepth, alpha, beta);
            int nextEval = newValue;
            updateWeights(currentEval, nextEval, evalParams);
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
    return finalValue;
}


int BinPackingGame::iterativeDeepeningMinimax(int maxDepth, float proportion = 0.2) {
    // Preprocess to handle perfect fits before starting the Minimax search
    preprocessPerfectFits(proportion);
    //preprocessTriplets(0.1);
    std::sort(items.begin(), items.end(), std::greater<int>());
    int bestValue = INT_MIN;
    for (int depth = 1; depth <= maxDepth; ++depth) {
        int value = hybridMethod(0, depth, INT_MIN, INT_MAX);
        if (value > bestValue) {
            bestValue = value;
        }
    }
    return bestValue;
}

void BinPackingGame::updatePerftTable(int depth) {
    if (depth >= perftTable.size()) {
        perftTable.resize(depth + 1, 0);
    }
    perftTable[depth]++;
}



std::set<std::vector<std::vector<int>>> uniqueConfigurations;

void BinPackingGame::trackConfiguration(const std::vector<Bin>& bins) {
    std::vector<std::vector<int>> currentConfig;
    for (const auto& bin : bins) {
        currentConfig.push_back(bin.contents); // Assuming getItems() returns the list of items in the bin
    }
    // Sort bins and their contents to ensure unique configurations are detected
    for (auto& config : currentConfig) {
        std::sort(config.begin(), config.end());
    }
    std::sort(currentConfig.begin(), currentConfig.end());
    uniqueConfigurations.insert(currentConfig);
}

static int ffCount = 0;
static int bfCount = 0;
static int nfCount = 0;

bool BinPackingGame::checkAgainstHeuristics(const std::vector<std::pair<int, int>>& moveSequence) {
    // Generate heuristic sequences
    bool flag = false;
    std::vector<std::pair<int, int>> bestFitSequence = generateBestFitSequence();
    std::vector<std::pair<int, int>> firstFitSequence = generateFirstFitSequence();
    std::vector<std::pair<int, int>> nextFitSequence = generateNextFitSequence();

    // Convert vectors to sets to ignore order
    std::set<std::pair<int, int>> moveSet(moveSequence.begin(), moveSequence.end());
    std::set<std::pair<int, int>> bestFitSet(bestFitSequence.begin(), bestFitSequence.end());
    std::set<std::pair<int, int>> firstFitSet(firstFitSequence.begin(), firstFitSequence.end());
    std::set<std::pair<int, int>> nextFitSet(nextFitSequence.begin(), nextFitSequence.end());
    // for(auto i : bestFitSet){
    //     std::cout << "(" << i.first << ", " << i.second << ")";
    // }
    // std::cout << std::endl;
    // for(auto i : firstFitSet){
    //     std::cout << "(" << i.first << ", " << i.second << ")";
    // }
    // std::cout << std::endl;

    // for(auto i : nextFitSet){
    //     std::cout << "(" << i.first << ", " << i.second << ")";
    // }
    // Compare the current move sequence against the heuristic sequences
    if (moveSet == bestFitSet) {
        std::cout << "This branch follows the Best-Fit heuristic (order ignored)." << std::endl;
        bfCount++;
        flag = true;
    } 
    if (moveSet == nextFitSet) {
        std::cout << "This branch follows the Next-Fit heuristic (order ignored)." << std::endl;
        nfCount++;  
        flag = true;
    }
    if(moveSet == firstFitSet){
        std::cout << "This branch follows the First-Fit heuristic (order ignored)." << std::endl;
        ffCount++;
        flag = true;
    }

    return (flag);
}







std::vector<std::pair<int, int>> BinPackingGame::generateFirstFitSequence() {
    std::vector<std::pair<int, int>> firstFitSequence;

    // Assume bins are empty initially
    std::vector<Bin> ffBins;
    ffBins.emplace_back(binCapacity);  // Start with one empty bin

    for (size_t i = 0; i < items.size(); ++i) {
        bool placed = false;
        for (size_t j = 0; j < ffBins.size(); ++j) {
            if (ffBins[j].canAdd(items[i])) {
                ffBins[j].addItem(items[i]);
                firstFitSequence.emplace_back(i, j);
                placed = true;
                break;
            }
        }
        if (!placed) {
            // Place the item in a new bin
            ffBins.emplace_back(binCapacity);
            ffBins.back().addItem(items[i]);
            firstFitSequence.emplace_back(i, ffBins.size() - 1);
        }
    }

    return firstFitSequence;
}


std::vector<std::pair<int, int>> BinPackingGame::generateBestFitSequence() {
    std::vector<std::pair<int, int>> bestFitSequence;

    std::vector<Bin> bfBins;
    bfBins.emplace_back(binCapacity);  // Start with one empty bin

    for (size_t i = 0; i < items.size(); ++i) {
        int bestBinIndex = -1;
        int minSpaceLeft = binCapacity + 1;

        for (size_t j = 0; j < bfBins.size(); ++j) {
            int spaceLeft = bfBins[j].getResidualCapacity() - items[i];
            if (spaceLeft >= 0 && spaceLeft < minSpaceLeft) {
                bestBinIndex = j;
                minSpaceLeft = spaceLeft;
            }
        }

        if (bestBinIndex != -1) {
            bfBins[bestBinIndex].addItem(items[i]);
            bestFitSequence.emplace_back(i, bestBinIndex);
        } else {
            // Place the item in a new bin
            bfBins.emplace_back(binCapacity);
            bfBins.back().addItem(items[i]);
            bestFitSequence.emplace_back(i, bfBins.size() - 1);
        }
    }

    return bestFitSequence;
}

std::vector<std::pair<int, int>> BinPackingGame::generateNextFitSequence() {
    std::vector<std::pair<int, int>> nextFitSequence;

    std::vector<Bin> nfBins;
    nfBins.emplace_back(binCapacity);  // Start with one empty bin
    size_t currentBinIndex = 0;

    for (size_t i = 0; i < items.size(); ++i) {
        if (nfBins[currentBinIndex].canAdd(items[i])) {
            nfBins[currentBinIndex].addItem(items[i]);
            nextFitSequence.emplace_back(i, currentBinIndex);
        } else {
            // Move to a new bin
            nfBins.emplace_back(binCapacity);
            currentBinIndex++;
            nfBins[currentBinIndex].addItem(items[i]);
            nextFitSequence.emplace_back(i, currentBinIndex);
        }
    }

    return nextFitSequence;
}


std::vector<std::pair<int, int>> BinPackingGame::generateAllMoves() {
    std::vector<std::pair<int, int>> moves;
    for (size_t i = 0; i < items.size(); ++i) {
        if (!placedItems[i]) {
            // Generate moves for placing the item in existing bins
            for (size_t j = 0; j < bins.size(); ++j) {
                if (bins[j].canAdd(items[i])) {
                    moves.emplace_back(i, j);
                }
            }
            // Generate the option to place the item in a new bin
            moves.emplace_back(i, bins.size());
        }
    }
    return moves;
}


void BinPackingGame::printGameTree(int depth, const std::vector<std::pair<int, int>>& moveSequence, const std::string& heuristicMatch) {
    // Print indentation based on depth
    for (int i = 0; i < depth; ++i) {
        std::cout << "  "; // Two spaces for each level of depth
    }

    // Print the current sequence and whether it matches a heuristic
    std::cout << "Depth " << depth << ": Sequence ";
    for (const auto& move : moveSequence) {
        std::cout << "(" << move.first << "," << move.second << ") ";
    }
    std::cout << " -> " << heuristicMatch << std::endl;
}




int BinPackingGame::pureMinimaxSequenceTracking(int depth, int maxDepth, std::vector<std::pair<int, int>>& moveSequence) {
    updatePerftTable(depth);

    // Base case: if all items are placed or max depth is reached, save the configuration
    if (std::all_of(placedItems.begin(), placedItems.end(), [](bool v) { return v; }) || depth == maxDepth) {
        int currentEval = evaluateState(evalParams);
        updateBestConfiguration(currentEval);
        trackConfiguration(bins);
        if(bins.size() < bestBinCount || bestBinCount == 0){
            bestBinCount = bins.size();
            bestBins = bins;
        }
        // Save the current sequence of moves as a complete configuration
        std::string heuristicMatch;
        allConfigurations.push_back(moveSequence);
        if(checkAgainstHeuristics(moveSequence)){
            heuristicMatch = "Known";
        }

        // Print the game tree at the leaf node
        //printGameTree(depth, moveSequence, heuristicMatch);

        return currentEval;
    }

    int value = INT_MIN;

    // Generate possible moves
    std::vector<std::pair<int, int>> moves = generateAllMoves();  // Ensure all moves are generated

    // Explore each move
    for (const auto &move : moves) {
        int itemIndex = move.first;
        int binIndex = move.second;

        moveSequence.push_back(move);  // Track the move

        if (binIndex < bins.size()) {
            // Place in an existing bin
            bins[binIndex].addItem(items[itemIndex]);
            placedItems[itemIndex] = true;

            int newValue = pureMinimaxSequenceTracking(depth + 1, maxDepth, moveSequence);

            bins[binIndex].removeLastItem();
            placedItems[itemIndex] = false;

            value = std::max(value, newValue);
        } else {
            // Place in a new bin
            Bin newBin(binCapacity);
            newBin.addItem(items[itemIndex]);
            bins.push_back(newBin);
            placedItems[itemIndex] = true;

            int newValue = pureMinimaxSequenceTracking(depth + 1, maxDepth, moveSequence);

            bins.pop_back();
            placedItems[itemIndex] = false;

            value = std::max(value, newValue);
        }

        moveSequence.pop_back();  // Backtrack the move
    }

    return value;
}

void BinPackingGame::printAllConfigurations() const {
    // Use a set of sets to store unique configurations
    std::set<std::set<std::pair<int, int>>> uniqueConfigurations;

    // Convert each configuration to a set and add it to the uniqueConfigurations set
    for (const auto& config : allConfigurations) {
        std::set<std::pair<int, int>> configSet(config.begin(), config.end());
        uniqueConfigurations.insert(configSet);
    }

    // Print the unique configurations
    std::cout << "All Unique Configurations Found:" << std::endl;
    for (const auto& configSet : uniqueConfigurations) {
        for (const auto& move : configSet) {
            std::cout << "(" << move.first << "," << move.second << ") ";
        }
        std::cout << std::endl;
    }
    std::cout << "Total Unique Configurations: " << uniqueConfigurations.size() << std::endl;
    std::cout << "first fit count: " << ffCount << std::endl;
    std::cout << "best fit count: " << bfCount << std::endl;
    std::cout << "next fit count: " << nfCount << std::endl;
}











int BinPackingGame::pureMinimax(int depth, int maxDepth) {
    updatePerftTable(depth);  // Update perft table for current depth
    minimaxRunCounter++;  // Increment the counter each time minimax is called

    // Evaluate the current state
    int currentEval = evaluateState(evalParams);

    // Base case: if all items are placed or max depth is reached, return the evaluation
    if (std::all_of(placedItems.begin(), placedItems.end(), [](bool v) { return v; }) || depth == maxDepth) {
        updateBestConfiguration(currentEval);
        trackConfiguration(bins);  // Track the configuration
        return currentEval;
    }

    int value = INT_MIN;  // Since this is a maximizing scenario

    // Generate possible moves
    std::vector<std::pair<int, int>> moves;  // Pair of item index and bin index
    for (size_t i = 0; i < items.size(); ++i) {
        if (!placedItems[i]) {  // If the item has not been placed yet
            // Generate moves for placing the item in existing bins
            for (size_t j = 0; j < bins.size(); ++j) {
                if (bins[j].canAdd(items[i])) {
                    moves.emplace_back(i, j);
                }
            }
            // Generate the option to place the item in a new bin
            moves.emplace_back(i, bins.size());
        }
    }

    // Explore each move
    for (const auto &move : moves) {
        int itemIndex = move.first;
        int binIndex = move.second;

        if (binIndex < bins.size()) {
            // Place in an existing bin
            bins[binIndex].addItem(items[itemIndex]);
            placedItems[itemIndex] = true;

            int newValue = pureMinimax(depth + 1, maxDepth);

            bins[binIndex].removeLastItem();
            placedItems[itemIndex] = false;

            value = std::max(value, newValue);
        } else {
            // Place in a new bin
            Bin newBin(binCapacity);
            newBin.addItem(items[itemIndex]);
            bins.push_back(newBin);
            placedItems[itemIndex] = true;

            int newValue = pureMinimax(depth + 1, maxDepth);

            bins.pop_back();
            placedItems[itemIndex] = false;

            value = std::max(value, newValue);
        }
    }

    return value;
}
std::string canonicalizeConfiguration(const std::vector<Bin>& bins) {
    std::vector<std::string> sortedBins;
    for (const auto& bin : bins) {
        sortedBins.push_back(bin.canonicalize());
    }
    std::sort(sortedBins.begin(), sortedBins.end());
    std::string configuration;
    for (const auto& bin : sortedBins) {
        configuration += "[" + bin + "] ";
    }
    return configuration;
}

int BinPackingGame::pureMinimaxWithPrint(int depth, int maxDepth, std::set<std::string>& seenConfigurations) {
    updatePerftTable(depth);  // Update perft table for current depth
    minimaxRunCounter++;  // Increment the counter each time minimax is called

    // Evaluate the current state
    int currentEval = evaluateState(evalParams);
    
    // Canonicalize the current configuration
    std::string canonicalForm = canonicalizeConfiguration(bins);

    // If this configuration has been seen before, skip processing it
    if (seenConfigurations.find(canonicalForm) != seenConfigurations.end()) {
        return INT_MIN;  // Skip this configuration if it's already been processed
    }

    // Mark this configuration as seen
    seenConfigurations.insert(canonicalForm);

    // Print the current unique configuration
    std::cout << canonicalForm << std::endl;
    std::cout << "--------------------------" << std::endl;

    // Base case: if all items are placed or max depth is reached, return the evaluation
    if (std::all_of(placedItems.begin(), placedItems.end(), [](bool v) { return v; }) || depth == maxDepth) {
        updateBestConfiguration(currentEval);
        return currentEval;
    }

    int value = INT_MIN;  // Since this is a maximizing scenario

    // Generate possible moves
    std::vector<std::pair<int, int>> moves;  // Pair of item index and bin index
    for (size_t i = 0; i < items.size(); ++i) {
        if (!placedItems[i]) {  // If the item has not been placed yet
            // Generate moves for placing the item in existing bins
            for (size_t j = 0; j < bins.size(); ++j) {
                if (bins[j].canAdd(items[i])) {
                    moves.emplace_back(i, j);
                }
            }
            // Generate the option to place the item in a new bin
            moves.emplace_back(i, bins.size());
        }
    }

    // Explore each move
    for (const auto &move : moves) {
        int itemIndex = move.first;
        int binIndex = move.second;

        if (binIndex < bins.size()) {
            // Place in an existing bin
            bins[binIndex].addItem(items[itemIndex]);
            placedItems[itemIndex] = true;

            int newValue = pureMinimaxWithPrint(depth + 1, maxDepth, seenConfigurations);

            bins[binIndex].removeLastItem();
            placedItems[itemIndex] = false;

            value = std::max(value, newValue);
        } else {
            // Place in a new bin
            Bin newBin(binCapacity);
            newBin.addItem(items[itemIndex]);
            bins.push_back(newBin);
            placedItems[itemIndex] = true;

            int newValue = pureMinimaxWithPrint(depth + 1, maxDepth, seenConfigurations);

            bins.pop_back();
            placedItems[itemIndex] = false;

            value = std::max(value, newValue);
        }
    }

    return value;
}





int BinPackingGame::minimaxAlphaBeta(int depth, int maxDepth, int alpha, int beta) {
    updatePerftTable(depth);

    minimaxRunCounter++; // Increment the counter each time minimax is called

    size_t stateHash = hashState();
    if (stateCache.find(stateHash) != stateCache.end()) {
        return stateCache[stateHash];
    }

    int currentEval = evaluateState(evalParams);
    if (std::all_of(placedItems.begin(), placedItems.end(),
                    [](bool v) { return v; })) {
        updateBestConfiguration(currentEval);
        stateCache[stateHash] = currentEval;
        return currentEval;
    }

    int value = INT_MIN;

    // Generate and order moves
    std::vector<std::pair<int, int>> moves; // Pair of item index and bin index
    for (size_t i = 0; i < items.size(); ++i) {
        if (!placedItems[i]) {
            // Generate possible moves for this item
            for (size_t j = 0; j < bins.size(); ++j) {
                if (bins[j].canAdd(items[i])) {
                    moves.emplace_back(i, j);
                }
            }
            // Adding the option to place the item in a new bin
            moves.emplace_back(i, bins.size());
        }
    }

    // Explore each move with alpha-beta pruning
    for (const auto &move : moves) {
        int itemIndex = move.first;
        int binIndex = move.second;

        if (binIndex < bins.size()) {
            // Place in an existing bin
            bins[binIndex].addItem(items[itemIndex]);
            placedItems[itemIndex] = true;

            int newValue = minimaxAlphaBeta(depth + 1, maxDepth, alpha, beta);

            bins[binIndex].removeLastItem();
            placedItems[itemIndex] = false;

            value = std::max(value, newValue);
            alpha = std::max(alpha, value);
            if (beta <= alpha) {
                break; // Pruning
            }
        } else {
            // Place in a new bin
            Bin newBin(binCapacity);
            newBin.addItem(items[itemIndex]);
            bins.push_back(newBin);
            placedItems[itemIndex] = true;

            int newValue = minimaxAlphaBeta(depth + 1, maxDepth, alpha, beta);

            bins.pop_back();
            placedItems[itemIndex] = false;

            value = std::max(value, newValue);
            alpha = std::max(alpha, value);
            if (beta <= alpha) {
                break; // Pruning
            }
        }
    }

    stateCache[stateHash] = value; // Cache the evaluated state
    return value;
}



int BinPackingGame::minimaxAlphaBetaWithPrint(int depth, int maxDepth, int alpha, int beta, std::set<std::string>& seenConfigurations) {
    updatePerftTable(depth);
    minimaxRunCounter++;

    size_t stateHash = hashState();
    if (stateCache.find(stateHash) != stateCache.end()) {
        return stateCache[stateHash];
    }

    int currentEval = bins.size();

    if (std::all_of(placedItems.begin(), placedItems.end(), [](bool v) { return v; }) || depth == maxDepth) {
        updateBestConfiguration(currentEval);
        stateCache[stateHash] = currentEval;
        return currentEval;
    }

    std::string canonicalForm = canonicalizeConfiguration(bins);
    if (seenConfigurations.find(canonicalForm) != seenConfigurations.end()) {
        return INT_MAX; // Skip this configuration if it's already been processed
    }

    seenConfigurations.insert(canonicalForm);

    int value = INT_MAX; // We want to minimise the number of bins

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

    for (const auto &move : moves) {
        int itemIndex = move.first;
        int binIndex = move.second;

        if (binIndex < bins.size()) {
            bins[binIndex].addItem(items[itemIndex]);
            placedItems[itemIndex] = true;

            int newValue = minimaxAlphaBetaWithPrint(depth + 1, maxDepth, alpha, beta, seenConfigurations);

            bins[binIndex].removeLastItem();
            placedItems[itemIndex] = false;

            value = std::min(value, newValue);
            beta = std::min(beta, value);
            if (beta <= alpha) {
                break; // Pruning based on the number of bins
            }
        } else {
            Bin newBin(binCapacity);
            newBin.addItem(items[itemIndex]);
            bins.push_back(newBin);
            placedItems[itemIndex] = true;

            int newValue = minimaxAlphaBetaWithPrint(depth + 1, maxDepth, alpha, beta, seenConfigurations);

            bins.pop_back();
            placedItems[itemIndex] = false;

            value = std::min(value, newValue);
            beta = std::min(beta, value);
            if (beta <= alpha) {
                break; // Pruning based on the number of bins
            }
        }
    }

    stateCache[stateHash] = value; // Cache the evaluated state
    return value;
}



int BinPackingGame::evaluateState(const EvaluationParams& params) const {
    int score = 0;

    

    double averageFill = std::accumulate(bins.begin(), bins.end(), 0,
                                         [](int sum, const Bin &bin) {
                                             return sum + bin.sumContents();
                                         }) /
                         static_cast<double>(bins.size());

    double variance = 0.0;
    int maxUtilization = 0;
    int minUtilization = binCapacity;
    int totalResidualCapacity = 0;
    int imbalancePenalty = 0;
    int similarSizeBonus = 0;

    std::vector<int> binUtilizations;

    for (const Bin &bin : bins) {
        int binContentSum = bin.sumContents();
        int maxItem = bin.contents.empty() ? 0 : *std::max_element(bin.contents.begin(), bin.contents.end());
        int minItem = bin.contents.empty() ? 0 : *std::min_element(bin.contents.begin(), bin.contents.end());

        binUtilizations.push_back(binContentSum);

        // Calculate variance
        variance += (binContentSum - averageFill) * (binContentSum - averageFill);

        // Track max and min utilization
        maxUtilization = std::max(maxUtilization, binContentSum);
        minUtilization = std::min(minUtilization, binContentSum);

        // Track residual capacity
        totalResidualCapacity += bin.getResidualCapacity();

        // Count overfilled bins
        if (binContentSum > bin.capacity) {
            score -= params.overfillPenalty;  // Apply penalty for overfilled bins
        }

        // Count empty bins
        if (binContentSum == 0) {
            score -= params.emptyBinPenalty;  // Apply penalty for empty bins
        }

        // Reward perfectly filled bins
        if (bin.getResidualCapacity() == 0) {
            score += params.perfectFillBonus;
        }

        // Penalize bins with significant item size differences
        imbalancePenalty += (maxItem - minItem);

        // Reward bins with similar-sized items
        if ((maxItem - minItem) < params.smallDifferenceThreshold) {
            similarSizeBonus += params.similarSizeBonus;
        }

        // Penalize underutilized bins more heavily
        if (binContentSum < bin.capacity * 0.8) {
            score -= params.heavyUnderutilizationPenalty * (bin.capacity - binContentSum);
        } else if (binContentSum < bin.capacity) {
            score -= params.underutilizationPenalty * (bin.capacity - binContentSum);
        }

        // Penalize each bin based on its usage
        score -= params.binUsagePenalty;
    }

    // Safeguard against empty bins vector to avoid division by zero
    if (bins.size() > 0) {
        variance = std::sqrt(variance / bins.size());  // Use standard deviation
    } else {
        variance = 0.0;
    }

    score -= (variance < binCapacity * 0.2) ? params.variancePenalty * 0.5 : params.variancePenalty;

    // Penalize imbalance between bins
    if (!binUtilizations.empty()) {
        int maxUtilization = *std::max_element(binUtilizations.begin(), binUtilizations.end());
        int minUtilization = *std::min_element(binUtilizations.begin(), binUtilizations.end());
        int balancePenalty = std::abs(maxUtilization - minUtilization);
        score -= balancePenalty * params.balanceFactor;
    }

    // Add penalties for residual capacity (unused space in bins)
    score -= totalResidualCapacity * params.residualPenalty;

    // Reward bins with items that fit well together
    score += similarSizeBonus;

    // Combine penalties with a focus on reducing bins and residual capacity
    score -= bins.size() * bins.size() * totalResidualCapacity;

    return score;
}



void BinPackingGame::updateWeights(int currentEval, int nextEval, EvaluationParams& params) {
    int tdError = nextEval - currentEval;

    // Adjust the penalties based on the TD error
    params.binUsagePenalty += static_cast<int>(params.learningRate * tdError * (-1));
    params.underutilizationPenalty += static_cast<int>(params.learningRate * tdError * (-1));
    params.heavyUnderutilizationPenalty += static_cast<int>(params.learningRate * tdError * (-1));
    params.variancePenalty += static_cast<int>(params.learningRate * tdError * (-1));
    params.overfillPenalty += static_cast<int>(params.learningRate * tdError * (-1));
    params.emptyBinPenalty += static_cast<int>(params.learningRate * tdError * (-1));
    params.partialFillPenalty += static_cast<int>(params.learningRate * tdError * (-1));
    params.residualPenalty += static_cast<int>(params.learningRate * tdError * (-1));
    params.perfectFillBonus += static_cast<int>(params.learningRate * tdError);
    params.imbalanceFactor += static_cast<int>(params.learningRate * tdError * (-1));
    params.similarSizeBonus += static_cast<int>(params.learningRate * tdError);
    params.balanceFactor += static_cast<int>(params.learningRate * tdError * (-1));

    // Ensure the penalties remain within reasonable bounds (cap to 50% of original values)
    params.binUsagePenalty = std::max(0, std::min(params.binUsagePenalty, 100 / 2));
    params.underutilizationPenalty = std::max(0, std::min(params.underutilizationPenalty, 50 / 2));
    params.heavyUnderutilizationPenalty = std::max(0, std::min(params.heavyUnderutilizationPenalty, 80 / 2));
    params.variancePenalty = std::max(0, std::min(params.variancePenalty, 30 / 2));
    params.overfillPenalty = std::max(0, std::min(params.overfillPenalty, 200 / 2));
    params.emptyBinPenalty = std::max(0, std::min(params.emptyBinPenalty, 150 / 2));
    params.partialFillPenalty = std::max(0, std::min(params.partialFillPenalty, 70 / 2));
    params.residualPenalty = std::max(0, std::min(params.residualPenalty, 10 / 2));
    params.perfectFillBonus = std::max(0, std::min(params.perfectFillBonus, 100 / 2));
    params.imbalanceFactor = std::max(0, std::min(params.imbalanceFactor, 1 / 2));
    params.similarSizeBonus = std::max(0, std::min(params.similarSizeBonus, 50 / 2));
    params.balanceFactor = std::max(0, std::min(params.balanceFactor, 50 / 2));
}






