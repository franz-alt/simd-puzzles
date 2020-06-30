#ifndef SIMD_ALGORITHMS_SCALE_AVX2_FMA_HPP
#define SIMD_ALGORITHMS_SCALE_AVX2_FMA_HPP

#include <cstdint>

#ifdef ENABLE_SIMD_AVX2

namespace simd::algorithms::avx2 {

void scale_fma(std::uint8_t * src, std::size_t length, std::uint8_t * dst, double factor, std::uint8_t offset);

} // namespace simd::algorithms::avx2

#endif

#endif // SIMD_ALGORITHMS_SCALE_AVX2_FMA_HPP
