#ifndef BINPACKINGGAME_H
#define BINPACKINGGAME_H

#include <vector>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <random>
#include <unordered_map>
#include <string>
#include <limits>
#include "Bin.h" 
#include <set>
#include <string>

class EvaluationConfig {
public:
  bool useBinPenalty = true;
  bool useUnderutilizationPenalty = true;
  bool useOptimalUtilizationBonus = true;
  bool useExcessBinsPenalty = true;
  bool useUniformityPenalty = true;

  int binUsagePenalty = 75;
  int underutilizationPenalty = 20;
  int optimalUtilizationBonus = 50;
  int excessBinsPenalty = 20;
  int softLimit = 10;
  int optimalFillPercentage = 90;
};

struct EvaluationParams {
    int binUsagePenalty;
    int underutilizationPenalty;
    int heavyUnderutilizationPenalty;
    int variancePenalty;
    int overfillPenalty;
    int emptyBinPenalty;
    int partialFillPenalty;
    int residualPenalty;
    int perfectFillBonus;
    int imbalanceFactor;
    int similarSizeBonus;
    int smallDifferenceThreshold;
    int balanceFactor;
    double learningRate;

    EvaluationParams()
        : binUsagePenalty(100),
          underutilizationPenalty(50),
          heavyUnderutilizationPenalty(80),
          variancePenalty(30),
          overfillPenalty(200),
          emptyBinPenalty(150),
          partialFillPenalty(70),
          residualPenalty(10),
          perfectFillBonus(100),
          imbalanceFactor(1),
          similarSizeBonus(50),
          smallDifferenceThreshold(10),
          balanceFactor(50),
          learningRate(0.01) {}
};

struct Item{
    int size;
    int id;
};


class BinPackingGame {
private:
    std::vector<int> items;
    std::vector<Item> items2;
    std::vector<Bin> bins;
    std::vector<Bin> bestBins;
    std::vector<bool> placedItems;
    int bestBinCount = INT_MAX;

    int bestValue;
    int binCapacity;
    EvaluationParams evalParams;  // Add this line to declare evalParams
    int minimaxRunCounter; // Counter for the number of minimax runs
    std::vector<std::vector<std::pair<int, int>>> allConfigurations;
    EvaluationConfig config;
    std::unordered_map<size_t, int> stateCache;    // Cache for hashed states
    std::vector<std::vector<size_t>> zobristTable; // Zobrist key
    std::unordered_map<std::string, int> memo;
    void initializeZobristTable();

    void updateBestConfiguration(int eval);

public:
    BinPackingGame(const std::vector<int>& its, int cap);
    BinPackingGame(const std::vector<int>& its, int cap, int bestValue);
    std::vector<Bin> getBestBins() const;
    std::vector<int> perftTable;

    void firstFit();
    void firstFitDecreasing();
    void nextFit();
    void nextFitDecreasing();
    void worstFit();
    void worstFitDecreasing();
    void bestFit();
    void bestFitDecreasing();
    int iterativeDeepeningMinimax(int maxDepth, float proportion);
    void preprocessPerfectFits(double proportion);
    void preprocessTriplets(double proportion);
    int evaluateFutureImpact(int residualAfterPlacement, size_t i, size_t j, size_t k);
    void placeInBestFitOrNewBin(size_t itemIndex);
    int scoreMove(int itemIndex, int binIndex) const;
    bool validatePacking();

    int hybridMethod(int depth, int maxDepth, int alpha, int beta);
    void updatePerftTable(int depth);
    void printCurrentConfiguration(const std::vector<Bin> &bins);
    void trackConfiguration(const std::vector<Bin> &bins);
    bool checkAgainstHeuristics(const std::vector<std::pair<int, int>> &moveSequence);
    std::vector<std::pair<int, int>> generateFirstFitSequence();
    std::vector<std::pair<int, int>> generateBestFitSequence();
    std::vector<std::pair<int, int>> generateNextFitSequence();
    std::vector<std::pair<int, int>> generateAllMoves();
    void printGameTree(int depth, const std::vector<std::pair<int, int>> &moveSequence, const std::string &heuristicMatch);
    int pureMinimaxSequenceTracking(int depth, int maxDepth, std::vector<std::pair<int, int>> &moveSequence);
    void printAllConfigurations() const;
    int pureMinimax(int depth, int maxDepth);
    int pureMinimaxWithPrint(int depth, int maxDepth, std::set<std::string>& seenConfigurations);
    int minimaxAlphaBeta(int depth, int maxDepth, int alpha, int beta);
    int minimaxAlphaBetaWithPrint(int depth, int maxDepth, int alpha, int beta, std::set<std::string> &seenConfigurations);
    size_t hashState();

    void updateBestConfigurationBasedOnBins();
    int evaluateState(const EvaluationParams &params) const;
    void updateWeights(int currentEval, int nextEval, EvaluationParams &params);
    size_t hashState() const;
    std::set<std::vector<std::vector<int>>> uniqueConfigurations;
    void mtrp();

    void printBins() const;
};

#endif // BINPACKINGGAME_H
