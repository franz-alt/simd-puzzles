#ifndef SIMD_ALGORITHMS_SCALE_NAIVE_HPP
#define SIMD_ALGORITHMS_SCALE_NAIVE_HPP

#include <cstdint>

namespace simd::algorithms {

// Scale all values of 'src', add an 'offset' and store result at 'dst'. 'dst' has to be of equal size as 'src'.
void scale_naive(std::uint8_t * src, std::size_t length, std::uint8_t * dst, double factor, std::uint8_t offset);

} // namespace simd::algorithms

#endif // SIMD_ALGORITHMS_SCALE_NAIVE_HPP
