#ifndef DIGGLER_TIPSIFY_HPP
#define DIGGLER_TIPSIFY_HPP

#include <vector>

namespace diggler {
namespace Util {

class Tipsify {
public:
  struct Vector3 {
    int pos[3];
    Vector3() = default;
    inline Vector3(int pos[3]) : pos{pos[0], pos[1], pos[2]} {}
    inline int operator[](size_t idx) const { return pos[idx]; }
    inline int& operator[](size_t idx) { return pos[idx]; }
    inline int operator*() const { return pos[0]; }
  };
  /**
   * @brief Computes the Average Cache Miss Ratio for a given cache size and set of faces
   * @param indices Face indices list
   * @param cacheSize Cache size (in vertex)
   * @return ACMR
   */
  static double calcACMR(const std::vector<Vector3> &faces, int cacheSize);

  static std::vector<Vector3> tipsify(std::size_t vertexCount,
    const std::vector<Vector3> &indices, int cacheSize);
};

}
}

#endif /* DIGGLER_TIPSIFY_HPP */
