#include "BinPackingGame.h"



void BinPackingGame::bestFit() {
    bins.clear();
    for (int item : items) {
        int bestIndex = -1;
        int minResidue = binCapacity + 1;
        for (int i = 0; i < bins.size(); ++i) {
        int residue = bins[i].getResidualCapacity();
        if (bins[i].canAdd(item) && residue < minResidue) {
            bestIndex = i;
            minResidue = residue;
        }
        }
        if (bestIndex == -1) {
        bins.emplace_back(binCapacity);
        bestIndex = bins.size() - 1;
        }
        bins[bestIndex].addItem(item);
    }
    bestBins = bins;
}