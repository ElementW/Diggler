#ifndef DIGGLER_TIPSIFY_HPP
#define DIGGLER_TIPSIFY_HPP

#include <vector>

namespace Diggler {
namespace Util {

class Tipsify {
public:
  /**
   * @brief Computes the Average Cache Miss Ratio for a given cache size and set of faces
   * @param indices Face indices list
   * @param cacheSize Cache size (in vertex)
   * @return ACMR
   */
  static double calcACMR(const std::vector<int[3]> &faces, int cacheSize);

  static std::vector<int[3]> tipsify(std::size_t vertexCount,
    const std::vector<int[3]> &indices, int cacheSize);
};

}
}

#endif /* DIGGLER_TIPSIFY_HPP */
