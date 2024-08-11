#include "BinPackingGame.h"

void BinPackingGame::nextFit() {
    bins.clear();
    bins.emplace_back(binCapacity);
    for (int item : items) {
      if (!bins.back().canAdd(item)) {
        bins.emplace_back(binCapacity);
      }
      bins.back().addItem(item);
    }
    bestBins = bins;
  }