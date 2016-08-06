#ifndef DIGGLER_PLATFORM_MATH_HPP
#define DIGGLER_PLATFORM_MATH_HPP

namespace Diggler {

///
/// Real Modulus
/// @returns Real modulus operation result, as such mod(x,y) is always positive
///
constexpr int rmod(int x, int y) {
  const int ret = x % y;
  return (ret < 0) ? y+ret : ret;
}
float rmod(float x, float y);
double rmod(double x, double y);

///
/// Divide rounding down / Modulo quotient
/// @returns x/y rounded down / Q in modulus' A=B×Q+R equation
///
constexpr inline int divrd(int x, uint y) {
  return (x < 0) ? (x+1)/(int)y-1 : x/(int)y;
}

///
/// @returns Floored value of f, as an integer
/// @see ::std::floor For results as float or double
///
constexpr inline int floor(const float f) {
  return (f > 0) ? (int)f : ((int)f)-1;
}

///
/// @returns Ceiling value of f, as an integer
/// @see ::std::ceil For results as float or double
///
constexpr inline int ceil(const float f) {
  return (f < 0) ? (int)f : ((int)f)+1;
}

///
/// @returns 1×[sign of v]. 0 if v == 0.
///
constexpr inline int signum(float v) {
  return (v > 0) ? 1 : (v < 0) ? -1 : 0;
}

///
/// @returns the smallest positive t such that s+t*ds is an integer.
/// @see http://gamedev.stackexchange.com/questions/47362/cast-ray-to-select-block-in-voxel-game
///
constexpr float intbound(float s, float ds) {
  return (ds < 0) ? intbound(-s, -ds) : (1-rmod(s, 1.f))/ds;
}

}

#endif /* DIGGLER_PLATFORM_MATH_HPP */
