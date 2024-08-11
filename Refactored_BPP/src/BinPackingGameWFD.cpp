#include "BinPackingGame.h"

void BinPackingGame::worstFitDecreasing() {
    std::sort(items.begin(), items.end(), std::greater<int>());
    worstFit();
}