#include "BinPackingGame.h"
#include "TestData.h"
#include <gtest/gtest.h>
#include <iostream>

class BinPackingGameTest : public ::testing::TestWithParam<std::string> {
protected:
    void RunTest(const std::string& algorithm) {
        for (const auto& testCase : testCases) {
            BinPackingGame game(testCase.items, testCase.binCapacity);
            if (algorithm == "firstFit") {
                game.firstFit();
            } else if (algorithm == "nextFit") {
                game.nextFit();
            } else if (algorithm == "hybrid") {
                game.iterativeDeepeningMinimax(1000, 0.4);
            } else {
                FAIL() << "Unknown algorithm: " << algorithm;
            }
            const auto& bins = game.getBestBins();

            // Debug prints
            std::cout << "Test case: " << testCase.name << std::endl;
            std::cout << "Algorithm: " << algorithm << std::endl;
            std::cout << "Expected bins: " << testCase.expectedBins << ", Actual bins: " << bins.size() << std::endl;
            for (const auto& bin : bins) {
                std::cout << "Bin contents: ";
                bin.print();
            }
            std::cout << std::endl;

            EXPECT_EQ(bins.size(), testCase.expectedBins) << "Failed on test case: " << testCase.name;
        }
    }
};

TEST_P(BinPackingGameTest, Algorithms) {
    RunTest(GetParam());
}

INSTANTIATE_TEST_SUITE_P(
    Algorithms, BinPackingGameTest,
    ::testing::Values("firstFit", "nextFit", "hybrid")
);

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
