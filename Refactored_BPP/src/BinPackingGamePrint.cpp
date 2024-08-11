#include "BinPackingGame.h"

void BinPackingGame::printBins() const {
    for (const Bin& bin : this->getBestBins()) {
        bin.print();
    }
}
