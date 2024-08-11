#include "BinPackingGame.h"

void BinPackingGame::nextFitDecreasing() {
    std::sort(items.begin(), items.end(), std::greater<int>());
    nextFit();
  }