#include "Tipsify.hpp"

#include <deque>
#include <map>
#include <vector>
#include <iostream>

/*
 * Implementation of AMD's Tipsy algorithm
 * Ported from Go, original at https://github.com/tfmoraes/Tipsify-Go
 * The following code is licensed under the MIT License.
 * (So feel free to use it)
 */

/*func BuildAdjacency(V [][3]float64, I [][3]int) (A [][]int, L []int);

func Skip_dead_end(L []int, D *[]int, I [][3]int, i int) (int);

func Get_next_vertex(I [][3]int, i int, k int, N map[int]bool, C []int, s int, L []int, D *[]int) (int);
*/
std::pair<std::vector<std::vector<int>>, std::vector<int>>
  BuildAdjacency(std::size_t vertexCount, const std::vector<int[3]> &I) {
  std::vector<std::vector<int>> A(vertexCount);
  std::vector<int> L(vertexCount, 0);

  for (std::size_t i = 0; i < I.size(); ++i) {
    A[I[i][0]].push_back(i);
    A[I[i][1]].push_back(i);
    A[I[i][2]].push_back(i);

    ++L[I[i][0]];
    ++L[I[i][1]];
    ++L[I[i][2]];
  }

  return std::make_pair(A, L);
}

int Skip_dead_end(const std::vector<int> &L, std::vector<int> &D, int i) {
  std::vector<int> D_(D);
  for (; D_.size() > 0;) {
    int d = D_.back();
    D_.pop_back();
    if (L[d] > 0) {
      D = D_;
      return d;
    }
  }
  D = D_;

  for (; i < L.size();) {
    if (L[i] > 0) {
      return i;
    }
    ++i;
  }
  return -1;
}

int Get_next_vertex(const int i, int k, const std::map<int, bool> &N, const std::vector<int> &C, int s,
  const std::vector<int> &L, std::vector<int> &D) {
  int n = -1, p = -1, m = 0;
  for (auto iter = N.begin(); iter != N.end(); ++iter) {
    int v = iter->first;
    if (L[v] > 0) {
      p = 0;
      if (s - C[v] + 2 * L[v] <= k) {
        p = s-C[v];
      }
      if (p > m) {
        m = p;
        n = v;
      }
    }
  }
  if (n == -1) {
    n = Skip_dead_end(L, D, i);
  }
  return n;
}

namespace diggler {
namespace Util {

std::vector<int[3]> Tipsify::tipsify(std::size_t vertexCount, const std::vector<int[3]> &I, int k) {
  std::vector<std::vector<int>> A;
  std::vector<int> L;
  {
    std::pair<std::vector<std::vector<int>>, std::vector<int>> AL(BuildAdjacency(vertexCount, I));
    A = std::move(AL.first);
    L = std::move(AL.second);
  }
  std::vector<int> C(vertexCount), D;
  std::vector<bool> E(I.size());
  const int i = 1;
  int f = 0, s = k + 1, nf = 0;
  std::vector<int[3]> O(I.size());
  for (; f >= 0;) {
    std::map<int, bool> N;
    for (int t : A[f]) {
      if (!E[t]) {
        for (unsigned int nv = 0; nv < sizeof(I[t])/sizeof(*I[t]); ++nv) {
          int v = I[t][nv];
          O[nf][nv] = v;
          D.push_back(v);
          N[v] = true;
          L[v] = L[v] - 1;
          if (s - C[v] > k) {
            C[v] = s;
            ++s;
          }
        }
        E[t] = true;
        ++nf;
      }
    }
    f = Get_next_vertex(i, k, N, C, s, L, D);
  }
  return O;
}

// FIFO cache
struct Cache {
  using DataT = std::deque<int>;
  using SizeT = DataT::size_type;
  SizeT size;
  DataT data;

  Cache(SizeT size) : size(size) {}
  bool push(int val) {
    for (int i : data) {
      if (i == val) {
        return false;
      }
    }
    if (data.size() == size) {
      data.pop_front();
    }
    data.push_back(val);
    return true;
  }
};

double Tipsify::calcACMR(const std::vector<int[3]> &faces, int cacheSize) {
  if (faces.empty()) {
    return 0;
  }
  Cache cache(cacheSize);
  int misses = 0;
  for (const int *face : faces) {
    for (int i = 0; i < 3; ++i) {
      if (cache.push(face[i])) {
        ++misses;
      }
    }
  }
  return static_cast<double>(misses) / faces.size();
}

}
}
