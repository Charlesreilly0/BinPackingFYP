#include "Bin.h"

Bin::Bin(int cap) : capacity(cap) {}

bool Bin::canAdd(int item) const {
    return (std::accumulate(contents.begin(), contents.end(), 0) + item <= capacity);
}

void Bin::addItem(int item) {
    contents.push_back(item);
}

void Bin::removeLastItem() {
    if (!contents.empty()) {
        contents.pop_back();
    }
}

int Bin::sumContents() const {
    return std::accumulate(contents.begin(), contents.end(), 0);
}

int Bin::getResidualCapacity() const {
    return capacity - sumContents();
}

void Bin::print() const {
    std::cout << "[ ";
    for (auto item : contents) {
        std::cout << item << " ";
    }
    std::cout << "]" << std::endl;
}

std::string Bin::canonicalize() const {
    std::vector<int> sortedItems = contents;
    std::sort(sortedItems.begin(), sortedItems.end());
    std::string representation;
    for (int item : sortedItems) {
        representation += std::to_string(item);
    }
    return representation;
}
