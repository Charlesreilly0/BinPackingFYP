#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <sys/resource.h>
#include <unistd.h>
#include <vector>

// Function to get the current memory usage of the process
long getMemoryUsage() {
  struct rusage usage;
  getrusage(RUSAGE_SELF, &usage);
  return usage.ru_maxrss; // Memory usage in kilobytes
}

// Function prototypes
void chmtp(int n, const std::vector<int> &w, int c, int jdim, int &z) {
  if (n < 2 || n > jdim) {
    z = -1;
    return;
  }
  for (int i = 0; i < n; ++i) {
    if (w[i] <= 0 || w[i] > c) {
      z = -2;
      return;
    }
  }
  for (int i = 0; i < n - 1; ++i) {
    if (w[i] < w[i + 1]) {
      z = -3;
      return;
    }
  }
  z = 0;
}

void ffdls(int n, const std::vector<int> &w, int c, int &z, std::vector<int> &r,
           std::vector<int> &xstar, std::vector<int> &ls, std::vector<int> &lsb,
           int jdim) {
  std::vector<int> bin_remain(jdim, c);
  z = 0;

  for (int i = 0; i < n; ++i) {
    bool placed = false;
    for (int j = 0; j < z; ++j) {
      if (bin_remain[j] >= w[i]) {
        bin_remain[j] -= w[i];
        xstar[i] = j + 1;
        r[j] = bin_remain[j];
        placed = true;
        break;
      }
    }
    if (!placed) {
      if (z < jdim) {
        bin_remain[z] = c - w[i];
        xstar[i] = z + 1;
        r[z] = bin_remain[z];
        z++;
      } else {
        std::cerr << "Error: Exceeded the number of available bins."
                  << std::endl;
        return;
      }
    }
  }
}

void l2(int n, const std::vector<int> &w, int c, int &lbal, int jdim) {
  int isum = std::accumulate(w.begin(), w.end(), 0);
  lbal = (isum + c - 1) / c;
}

void l3(int n, const std::vector<int> &w, int c, int, int &m,
        std::vector<int> &dum, std::vector<int> &xstar, int &nf, int &lb3, int,
        std::vector<int> &xstarr, int isum, int &z, std::vector<int> &res,
        std::vector<int> &rel, int jdim) {
  m = 0;
  nf = n;
  lb3 = (isum + c - 1) / c;
}

void enumer(int nf, const std::vector<int> &wr, int c, std::vector<int> &xstarr,
            int &vstar, int &lb, int &back, std::vector<int> &x,
            std::vector<int> &r, std::vector<int> &wa, std::vector<int> &wb,
            std::vector<int> &kfix, std::vector<int> &fixit,
            std::vector<int> &xred, std::vector<int> &ls, std::vector<int> &lsb,
            std::vector<int> &dum, std::vector<int> &xheu,
            std::vector<int> &res, std::vector<int> &rel, int jdim) {
  vstar = (nf + c - 1) / c;
}

// Main function implementing the MTP algorithm
void mtp(int n, std::vector<int> &w, int c, int jdim, int &back, int jck,
         int &z, std::vector<int> &xstar, int &lb) {
  std::vector<int> wr(jdim), xstarr(jdim), dum(jdim), res(jdim), rel(jdim),
      x(jdim), r(jdim), wa(jdim), wb(jdim);
  std::vector<int> kfix(jdim), fixit(jdim), xred(jdim), ls(jdim), lsb(jdim),
      xheu(jdim);

  z = 0;
  if (jck == 1)
    chmtp(n, w, c, jdim, z);
  if (z < 0)
    return;

  int lbstar = 0;
  int nn = 9 * n / 10;
  if (w[0] + w[nn] >= c) {
    int isum = std::accumulate(w.begin(), w.begin() + n, 0);
    z = 0;
    int nf, lb3;
    int m = 0;
    l3(n, w, c, 0, m, dum, xstar, nf, lb3, n + 1, xstarr, isum, z, res, rel,
       jdim);
    if (lb3 > lbstar)
      lbstar = lb3;
    if (nf > 0) {
      if (nf == n) {
        z = lb3;
        lb = lbstar;
        return;
      }

      int ii = 0;
      for (int i = 0; i < n; ++i) {
        if (xstar[i] <= 0) {
          wr[ii] = w[i];
          xstarr[ii++] = xstarr[i] - m;
        }
      }

      int vstar = z - m;
      lb = lbstar - m;
      enumer(nf, wr, c, xstarr, vstar, lb, back, x, r, wa, wb, kfix, fixit,
             xred, ls, lsb, dum, xheu, res, rel, jdim);

      z = vstar + m;
      lb = lb + m;
      ii = 0;
      for (int i = 0; i < n; ++i) {
        if (xstar[i] <= 0) {
          xstar[i] = xstarr[ii++] + m;
        }
      }
      return;
    }
  } else {
    ffdls(n, w, c, z, r, xstar, ls, lsb, jdim);

    int isumr = std::accumulate(r.begin(), r.begin() + z, 0);
    int isum = z * c - isumr;
    lbstar = (isum - 1) / c + 1;
    if (lbstar == z) {
      back = 0;
      lb = lbstar;
      return;
    }

    int iss = 0;
    for (int i = 0; i < n; ++i) {
      if (w[i] + w[n - 1] <= c) {
        iss = isum - iss;
        lbstar = (i - 1) + (iss - 1) / c + 1;
        if (lbstar == z) {
          back = 0;
          lb = lbstar;
          return;
        }
        break;
      }
      iss += w[i];
    }

    int lbal;
    l2(n, w, c, lbal, jdim);
    if (lbal <= lbstar) {
      back = 0;
      lb = lbstar;
      return;
    }
    lbstar = lbal;
    if (lbstar == z) {
      back = 0;
      lb = lbstar;
      return;
    }
  }
}

// int main(int argc, char *argv[]) {
//   if (argc < 4) {
//     std::cerr << "Usage: " << argv[0]
//               << " <bin_capacity> <best_known_bins> <item_sizes>\n";
//     return 1;
//   }

//   int binCapacity = std::stoi(argv[1]);
//   int bestKnownBins = std::stoi(argv[2]);

//   std::vector<int> items;
//   std::stringstream ss(argv[3]);
//   std::string item;
//   while (std::getline(ss, item, ',')) {
//     items.push_back(std::stoi(item));
//   }
//   std::sort(items.begin(), items.end(), std::greater<>());
//   int n = items.size();
//   int jdim = n;
//   int back = -1;
//   int jck = 1;
//   int z;
//   std::vector<int> xstar(jdim, 0);
//   int lb;

//   // Measure initial memory usage
//   long initialMemory = getMemoryUsage();

//   auto start = std::chrono::high_resolution_clock::now();

//   mtp(n, items, binCapacity, jdim, back, jck, z, xstar, lb);

//   auto end = std::chrono::high_resolution_clock::now();
//   std::chrono::duration<double, std::milli> duration = end - start;

//   // Measure final memory usage
//   long finalMemory = getMemoryUsage();
//   long memoryUsed = finalMemory - initialMemory;

//   auto printResults = [bestKnownBins, memoryUsed](const std::string &algorithm,
//                                                   int binsUsed, double runtime,
//                                                   int itemsCount) {
//     std::cout << "{\"Algorithm\":\"" << algorithm
//               << "\", \"BinsUsed\":" << binsUsed << ", \"Runtime\":" << runtime
//               << ", \"MemoryUsed\":" << memoryUsed
//               << ", \"ItemsCount\":" << itemsCount << "}" << std::endl;
//   };

//   printResults("MTP", z, duration.count(), n);

//   return 0;
// }
