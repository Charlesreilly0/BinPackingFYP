#include "BinPackingGame.h"

void BinPackingGame::firstFitDecreasing() {
    std::sort(items.begin(), items.end(), std::greater<int>());
    firstFit();
  }