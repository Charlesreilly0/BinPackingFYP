#include "BinPackingGame.h"
  
  
void BinPackingGame::bestFitDecreasing() {
    std::sort(items.begin(), items.end(), std::greater<int>());
    bestFit();
}