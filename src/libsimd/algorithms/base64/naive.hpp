#ifndef SIMD_ALGORITHMS_BASE64_NAIVE_HPP
#define SIMD_ALGORITHMS_BASE64_NAIVE_HPP

#include <cstdint>

namespace simd::algorithms {

// Calculate Base64 representataion of 'src' and store result at 'dst'. 'dst' has to be of adequate size.
void to_base64_naive(std::uint8_t * src, std::size_t length, std::size_t padding, std::uint8_t * dst);

} // namespace simd::algorithms

#endif // SIMD_ALGORITHMS_BASE64_NAIVE_HPP
