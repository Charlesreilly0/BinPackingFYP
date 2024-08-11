#include "BinPackingGame.h"

void BinPackingGame::firstFit() {
    bins.clear();
    for (int item : items) {
      bool placed = false;
      for (Bin &bin : bins) {
        if (bin.canAdd(item)) {
          bin.addItem(item);
          placed = true;
          break;
        }
      }
      if (!placed) {
        bins.emplace_back(binCapacity);
        bins.back().addItem(item);
      }
    }
    bestBins = bins;
  }