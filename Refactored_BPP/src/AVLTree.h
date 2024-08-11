#ifndef AVLTREE_H
#define AVLTREE_H

#include <vector>
#include <iostream>
#include <algorithm>

// AVL Node structure
struct AVLNode {
    int binCapacity;
    std::vector<int> items;  // Store items packed in this bin
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(int capacity);
};

// AVL Tree class
class AVLTree {
public:
    AVLTree();
    ~AVLTree();

    void insert(int binCapacity, int item);
    void remove(int binCapacity);
    int firstFitDecreasing(std::vector<int>& items, int binCapacity);
    int bestFitDecreasing(std::vector<int>& items, int binCapacity);
    int worstFitDecreasing(std::vector<int>& items, int binCapacity);
    int nextFitDecreasing(std::vector<int>& items, int binCapacity);
    int printBins();
    int firstFit(std::vector<int> &items, int binCapacity);
    int bestFit(std::vector<int> &items, int binCapacity);
    int nextFit(std::vector<int> &items, int binCapacity);
    int worstFit(std::vector<int> &items, int binCapacity);
    void clear();

private:
    AVLNode* root;

    AVLNode* insert(AVLNode* node, int binCapacity, int item);
    AVLNode* remove(AVLNode* node, int binCapacity);
    AVLNode* findMin(AVLNode* node);
    AVLNode* rotateRight(AVLNode* y);
    AVLNode* rotateLeft(AVLNode* x);
    int getHeight(AVLNode* node);
    int getBalance(AVLNode* node);
    void storeBinItems(AVLNode* node, std::vector<std::vector<int>>& bins);
    AVLNode* findBestFit(AVLNode* node, int itemSize);
    AVLNode* findWorstFit(AVLNode* node);
    void clear(AVLNode* node);
};

#endif // AVLTREE_H
