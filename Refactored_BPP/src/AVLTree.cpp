#include "AVLTree.h"
#include <iostream>
#include <algorithm>

// AVLNode constructor
AVLNode::AVLNode(int capacity) : binCapacity(capacity), left(nullptr), right(nullptr), height(1) {}

// AVLTree constructor
AVLTree::AVLTree() : root(nullptr) {}

// AVLTree destructor
AVLTree::~AVLTree() {
    clear(root);
}

// Insert a bin with capacity and item
AVLNode* AVLTree::insert(AVLNode* node, int binCapacity, int item) {
    if (node == nullptr) {
        AVLNode* newNode = new AVLNode(binCapacity);
        newNode->items.push_back(item);
        return newNode;
    }

    if (binCapacity < node->binCapacity) {
        node->left = insert(node->left, binCapacity, item);
    } else {
        node->right = insert(node->right, binCapacity, item);
    }

    node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
    int balance = getBalance(node);

    if (balance > 1 && binCapacity < node->left->binCapacity) return rotateRight(node);
    if (balance < -1 && binCapacity >= node->right->binCapacity) return rotateLeft(node);
    if (balance > 1 && binCapacity >= node->left->binCapacity) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1 && binCapacity < node->right->binCapacity) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

// Public method to insert
void AVLTree::insert(int binCapacity, int item) {
    root = insert(root, binCapacity, item);
}

// Remove a bin with capacity
AVLNode* AVLTree::remove(AVLNode* node, int binCapacity) {
    if (node == nullptr) return node;

    if (binCapacity < node->binCapacity) {
        node->left = remove(node->left, binCapacity);
    } else if (binCapacity > node->binCapacity) {
        node->right = remove(node->right, binCapacity);
    } else {
        if (node->left == nullptr || node->right == nullptr) {
            AVLNode* temp = node->left ? node->left : node->right;
            if (temp == nullptr) {
                temp = node;
                node = nullptr;
            } else {
                *node = *temp;
            }
            delete temp;
        } else {
            AVLNode* temp = findMin(node->right);
            node->binCapacity = temp->binCapacity;
            node->items = temp->items;
            node->right = remove(node->right, temp->binCapacity);
        }
    }

    if (node == nullptr) return node;

    node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
    int balance = getBalance(node);

    if (balance > 1 && getBalance(node->left) >= 0) return rotateRight(node);
    if (balance > 1 && getBalance(node->left) < 0) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1 && getBalance(node->right) <= 0) return rotateLeft(node);
    if (balance < -1 && getBalance(node->right) > 0) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

// Public method to remove
void AVLTree::remove(int binCapacity) {
    root = remove(root, binCapacity);
}

// Find the minimum value node
AVLNode* AVLTree::findMin(AVLNode* node) {
    AVLNode* current = node;
    while (current->left != nullptr) current = current->left;
    return current;
}

// Get height of a node
int AVLTree::getHeight(AVLNode* node) {
    return node == nullptr ? 0 : node->height;
}

// Get balance factor of a node
int AVLTree::getBalance(AVLNode* node) {
    return node == nullptr ? 0 : getHeight(node->left) - getHeight(node->right);
}

// Right rotate
AVLNode* AVLTree::rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = std::max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = std::max(getHeight(x->left), getHeight(x->right)) + 1;

    return x;
}

// Left rotate
AVLNode* AVLTree::rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = std::max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = std::max(getHeight(y->left), getHeight(y->right)) + 1;

    return y;
}

// Store items in each bin
void AVLTree::storeBinItems(AVLNode* node, std::vector<std::vector<int>>& bins) {
    if (node == nullptr) return;

    storeBinItems(node->left, bins);
    if (!node->items.empty()) {
        bins.push_back(node->items);
    }
    storeBinItems(node->right, bins);
}

// Print bins configuration and return the number of bins
int AVLTree::printBins() {
    std::vector<std::vector<int>> bins;
    storeBinItems(root, bins);
    int binCount = bins.size();
    for (const auto& bin : bins) {
        std::cout << "[";
        for (size_t i = 0; i < bin.size(); ++i) {
            std::cout << bin[i];
            if (i < bin.size() - 1) std::cout << ",";
        }
        std::cout << "] ";
    }
    std::cout << std::endl;
    return binCount;
}

// First Fit Decreasing heuristic
int AVLTree::firstFitDecreasing(std::vector<int>& items, int binCapacity) {
    std::sort(items.begin(), items.end(), std::greater<int>());
    int binCount = 0; // Track the number of bins used

    for (int item : items) {
        AVLNode* node = root;
        AVLNode* bestFitBin = nullptr;

        while (node != nullptr) {
            if (item <= node->binCapacity) {
                bestFitBin = node;
                node = node->left;
            } else {
                node = node->right;
            }
        }

        if (bestFitBin == nullptr) {
            binCount++; // New bin created
            std::cout << "No fitting bin found for item " << item << ". Creating new bin." << std::endl;
            insert(binCapacity - item, item);
        } else {
            int newCapacity = bestFitBin->binCapacity - item;
            std::cout << "Inserting item " << item << " into bin with remaining capacity " << bestFitBin->binCapacity << std::endl;
            remove(bestFitBin->binCapacity);
            if (newCapacity > 0) insert(newCapacity, item);
        }
    }

    printBins(); // Use inorder-traversal to print bins
    return binCount; // Return the number of bins used
}

// Best Fit Decreasing heuristic
int AVLTree::bestFitDecreasing(std::vector<int>& items, int binCapacity) {
    std::sort(items.begin(), items.end(), std::greater<int>());
    int binCount = 0; // Track the number of bins used

    for (int item : items) {
        AVLNode* bestFitBin = nullptr;
        AVLNode* node = root;

        while (node != nullptr) {
            if (item <= node->binCapacity) {
                if (bestFitBin == nullptr || node->binCapacity < bestFitBin->binCapacity) {
                    bestFitBin = node;
                }
                node = node->left;
            } else {
                node = node->right;
            }
        }

        if (bestFitBin == nullptr) {
            binCount++; // New bin created
            std::cout << "No fitting bin found for item " << item << ". Creating new bin." << std::endl;
            insert(binCapacity - item, item);
        } else {
            int newCapacity = bestFitBin->binCapacity - item;
            std::cout << "Inserting item " << item << " into best fitting bin with remaining capacity " << bestFitBin->binCapacity << std::endl;
            remove(bestFitBin->binCapacity);
            if (newCapacity > 0) insert(newCapacity, item);
        }
    }

    printBins();
    return binCount; // Return the number of bins used
}

// Worst Fit Decreasing heuristic
int AVLTree::worstFitDecreasing(std::vector<int>& items, int binCapacity) {
    std::sort(items.begin(), items.end(), std::greater<int>());
    int binCount = 0; // Track the number of bins used

    for (int item : items) {
        AVLNode* worstFitBin = nullptr;
        AVLNode* node = root;

        while (node != nullptr) {
            if (item <= node->binCapacity) {
                if (worstFitBin == nullptr || node->binCapacity > worstFitBin->binCapacity) {
                    worstFitBin = node;
                }
                node = node->left;
            } else {
                node = node->right;
            }
        }

        if (worstFitBin == nullptr) {
            binCount++; // New bin created
            std::cout << "No fitting bin found for item " << item << ". Creating new bin." << std::endl;
            insert(binCapacity - item, item);
        } else {
            int newCapacity = worstFitBin->binCapacity - item;
            std::cout << "Inserting item " << item << " into worst fitting bin with remaining capacity " << worstFitBin->binCapacity << std::endl;
            remove(worstFitBin->binCapacity);
            if (newCapacity > 0) insert(newCapacity, item);
        }
    }

    printBins();
    return binCount; // Return the number of bins used
}

// Next Fit Decreasing heuristic
int AVLTree::nextFitDecreasing(std::vector<int>& items, int binCapacity) {
    std::sort(items.begin(), items.end(), std::greater<int>());
    int binCount = 0; // Track the number of bins used
    int currentBinCapacity = binCapacity;

    for (int item : items) {
        if (item <= currentBinCapacity) {
            std::cout << "Inserting item " << item << " into current bin with remaining capacity " << currentBinCapacity << std::endl;
            currentBinCapacity -= item;
        } else {
            binCount++; // New bin created
            std::cout << "Current bin full. Creating new bin for item " << item << std::endl;
            currentBinCapacity = binCapacity - item;
        }
    }

    binCount++; // Count the last bin used
    printBins();
    return binCount; // Return the number of bins used
}

// Clear the entire tree
void AVLTree::clear(AVLNode* node) {
    if (node != nullptr) {
        clear(node->left);
        clear(node->right);
        delete node;
    }
}

int AVLTree::firstFit(std::vector<int>& items, int binCapacity) {
    int binCount = 0; // Track the number of bins used

    for (int item : items) {
        AVLNode* node = root;
        AVLNode* firstFitBin = nullptr;

        while (node != nullptr) {
            if (item <= node->binCapacity) {
                firstFitBin = node;
                break; // Find the first bin that fits and break
            }
            node = node->left ? node->left : node->right;
        }

        if (firstFitBin == nullptr) {
            binCount++; // New bin created
            std::cout << "No fitting bin found for item " << item << ". Creating new bin." << std::endl;
            insert(binCapacity - item, item);
        } else {
            int newCapacity = firstFitBin->binCapacity - item;
            std::cout << "Inserting item " << item << " into bin with remaining capacity " << firstFitBin->binCapacity << std::endl;
            remove(firstFitBin->binCapacity);
            if (newCapacity > 0) insert(newCapacity, item);
        }
    }

    printBins(); // Use inorder-traversal to print bins
    return binCount; // Return the number of bins used
}
int AVLTree::bestFit(std::vector<int>& items, int binCapacity) {
    int binCount = 0; // Track the number of bins used

    for (int item : items) {
        AVLNode* node = root;
        AVLNode* bestFitBin = nullptr;

        while (node != nullptr) {
            if (item <= node->binCapacity) {
                if (bestFitBin == nullptr || node->binCapacity < bestFitBin->binCapacity) {
                    bestFitBin = node; // Update to the best fit
                }
            }
            node = node->left ? node->left : node->right;
        }

        if (bestFitBin == nullptr) {
            binCount++; // New bin created
            std::cout << "No fitting bin found for item " << item << ". Creating new bin." << std::endl;
            insert(binCapacity - item, item);
        } else {
            int newCapacity = bestFitBin->binCapacity - item;
            std::cout << "Inserting item " << item << " into bin with remaining capacity " << bestFitBin->binCapacity << std::endl;
            remove(bestFitBin->binCapacity);
            if (newCapacity > 0) insert(newCapacity, item);
        }
    }

    printBins(); // Use inorder-traversal to print bins
    return binCount; // Return the number of bins used
}
int AVLTree::nextFit(std::vector<int>& items, int binCapacity) {
    int binCount = 0; // Track the number of bins used
    int currentBinCapacity = binCapacity;

    for (int item : items) {
        if (item <= currentBinCapacity) {
            currentBinCapacity -= item;
            std::cout << "Inserting item " << item << " into current bin with remaining capacity " << currentBinCapacity << std::endl;
        } else {
            binCount++; // New bin created
            currentBinCapacity = binCapacity - item;
            std::cout << "No space in current bin for item " << item << ". Creating new bin with capacity " << currentBinCapacity << std::endl;
        }
    }

    printBins(); // Use inorder-traversal to print bins (if applicable)
    return binCount + 1; // Return the number of bins used (including the last one)
}
int AVLTree::worstFit(std::vector<int>& items, int binCapacity) {
    int binCount = 0; // Track the number of bins used

    for (int item : items) {
        AVLNode* node = root;
        AVLNode* worstFitBin = nullptr;

        while (node != nullptr) {
            if (item <= node->binCapacity) {
                if (worstFitBin == nullptr || node->binCapacity > worstFitBin->binCapacity) {
                    worstFitBin = node; // Update to the worst fit
                }
            }
            node = node->left ? node->left : node->right;
        }

        if (worstFitBin == nullptr) {
            binCount++; // New bin created
            std::cout << "No fitting bin found for item " << item << ". Creating new bin." << std::endl;
            insert(binCapacity - item, item);
        } else {
            int newCapacity = worstFitBin->binCapacity - item;
            std::cout << "Inserting item " << item << " into bin with remaining capacity " << worstFitBin->binCapacity << std::endl;
            remove(worstFitBin->binCapacity);
            if (newCapacity > 0) insert(newCapacity, item);
        }
    }

    printBins(); // Use inorder-traversal to print bins
    return binCount; // Return the number of bins used
}


void AVLTree::clear() {
    clear(root);
    root = nullptr;
}
