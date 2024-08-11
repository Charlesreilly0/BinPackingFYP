#include "BinPackingGame.h"

void BinPackingGame::mtrp() {
    int n = items.size();
    std::vector<int> N(n);
    std::iota(N.begin(), N.end(), 0);

    while (!N.empty()) {
        int j = *std::min_element(N.begin(), N.end(), [this](int a, int b) {
            return items[a] < items[b];
        });

        auto N_prime = N;
        N_prime.erase(std::remove(N_prime.begin(), N_prime.end(), j), N_prime.end());
        std::sort(N_prime.begin(), N_prime.end(), [this](int a, int b) { return items[a] > items[b]; });

        std::vector<int> F;
        int k = 0;
        for (int i = 0; i < N_prime.size(); ++i) {
            int sum = items[j];
            for (int l = 0; l < k; ++l) {
                sum += items[N_prime[l]];
            }
            if (sum <= binCapacity) {
                k = i + 1;
            } else {
                break;
            }
        }

        if (k == 0) {
            F.push_back(j);
        } else {
            if (k == 1) {
                int j_star = *std::min_element(
                    N_prime.begin(), N_prime.end(), [this, j](int a, int b) {
                        return items[j] + items[a] <= binCapacity &&
                               items[j] + items[a] <= items[j] + items[b];
                    });
                F = {j, j_star};
            } else if (k == 2) {
                int j_a = 0, j_b = 0;
                int max_weight = std::numeric_limits<int>::min();
                for (int a = 0; a < N_prime.size(); ++a) {
                    for (int b = a + 1; b < N_prime.size(); ++b) {
                        int w_sum = items[N_prime[a]] + items[N_prime[b]];
                        if (w_sum <= binCapacity && w_sum > max_weight) {
                            max_weight = w_sum;
                            j_a = N_prime[a];
                            j_b = N_prime[b];
                        }
                    }
                }

                if (items[j] + items[j_a] + items[j_b] <= binCapacity) {
                    F = {j, j_a, j_b};
                } else if (items[j] + items[j_a] <= binCapacity) {
                    F = {j, j_a};
                }
            }
        }

        if (F.empty()) {
            bins.emplace_back(binCapacity);
            bins.back().addItem(items[j]);
            N.erase(std::remove(N.begin(), N.end(), j), N.end());
        } else {
            bins.emplace_back(binCapacity);
            for (auto& f : F) {
                bins.back().addItem(items[f]);
                N.erase(std::remove(N.begin(), N.end(), f), N.end());
            }
        }
    }

    if (bins.size() < bestValue) {
        bestValue = bins.size();
        bestBins = bins;
    }
}
