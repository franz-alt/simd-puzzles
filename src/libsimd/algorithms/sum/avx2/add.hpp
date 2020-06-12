#ifndef SIMD_ALGORITHMS_SUM_AVX2_ADD_HPP
#define SIMD_ALGORITHMS_SUM_AVX2_ADD_HPP

#include <cstdint>

#ifdef ENABLE_SIMD_AVX2

namespace simd::algorithms::avx2 {

std::uint32_t sum_add(std::uint8_t * data, std::size_t size);

} // namespace simd::algorithms::avx2

#endif

#endif // SIMD_ALGORITHMS_SUM_AVX2_ADD_HPP
