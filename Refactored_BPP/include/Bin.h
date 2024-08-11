#ifndef BIN_H
#define BIN_H

#include <vector>
#include <numeric>
#include <iostream>

class Bin {
public:
    int capacity;
    std::vector<int> contents;

    Bin(int cap);
    
    bool canAdd(int item) const;
    void addItem(int item);
    void removeLastItem();
    int sumContents() const;
    int getResidualCapacity() const;
    void print() const;
    std::string canonicalize() const;
};

#endif // BIN_H
