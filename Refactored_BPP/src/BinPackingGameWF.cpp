#include "BinPackingGame.h"
void BinPackingGame::worstFit() {
    bins.clear();
    for (int item : items) {
        int worstIndex = -1;
        int maxResidue = -1;
        for (int i = 0; i < bins.size(); ++i) {
        int residue = bins[i].getResidualCapacity();
        if (bins[i].canAdd(item) && residue > maxResidue) {
            worstIndex = i;
            maxResidue = residue;
        }
        }
        if (worstIndex == -1) {
        bins.emplace_back(binCapacity);
        worstIndex = bins.size() - 1;
        }
        bins[worstIndex].addItem(item);
    }
    bestBins = bins;
}