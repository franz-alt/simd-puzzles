#ifndef SIMD_ALGORITHMS_BASE64_AVX2_SHUFFLE_HPP
#define SIMD_ALGORITHMS_BASE64_AVX2_SHUFFLE_HPP

#include <cstdint>

namespace simd::algorithms::avx2 {

// Calculate Base64 representataion of 'src' and store result at 'dst'. 'dst' has to be of adequate size.
void to_base64_shuffle(std::uint8_t * src, std::size_t length, std::size_t padding, std::uint8_t * dst);

// Calculate ASCII representation of 'src' and store result at 'dst'. 'dst' has to be of adequate size.
std::size_t from_base64_shuffle(std::uint8_t * src, std::size_t length, std::uint8_t * dst);

} // namespace simd::algorithms::avx2

#endif // SIMD_ALGORITHMS_BASE64_AVX2_SHUFFLE_HPP
