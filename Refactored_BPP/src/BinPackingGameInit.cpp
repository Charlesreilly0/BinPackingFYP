#include "BinPackingGame.h"

BinPackingGame::BinPackingGame(const std::vector<int>& its, int cap)
    : items(its), binCapacity(cap), bestValue(INT_MAX), minimaxRunCounter(0) {
    placedItems.resize(items.size(), false);
    zobristTable.resize(items.size(), std::vector<size_t>(bins.size() + 1));
    initializeZobristTable();
}

BinPackingGame::BinPackingGame(const std::vector<int>& its, int cap, int bestValue)
    : items(its), binCapacity(cap), bestValue(INT_MAX), minimaxRunCounter(0) {
    placedItems.resize(items.size(), false);
    zobristTable.resize(items.size(), std::vector<size_t>(bins.size() + 1));
    initializeZobristTable();
}

std::vector<Bin> BinPackingGame::getBestBins() const { return bestBins; }

void BinPackingGame::initializeZobristTable() {
    std::mt19937_64 rng(std::random_device{}());
    std::uniform_int_distribution<size_t> dist(
        0, std::numeric_limits<size_t>::max());

    for (size_t i = 0; i < items.size(); ++i) {
        for (size_t j = 0; j <= bins.size(); ++j) {
        zobristTable[i][j] = dist(rng);
        }
    }
}

void BinPackingGame::updateBestConfiguration(int eval) {
    if(eval < bestValue) {
        bestValue = eval;
        bestBins = bins;
    }
}

std::vector<Item> initializeItems(const std::vector<int>& itemSizes) {
    std::vector<Item> items2;
    for (size_t i = 0; i < itemSizes.size(); ++i) {
        items2.push_back(Item{itemSizes[i], static_cast<int>(i)});
    }
    return items2;
}
