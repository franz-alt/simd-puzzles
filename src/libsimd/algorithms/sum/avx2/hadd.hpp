#ifndef SIMD_ALGORITHMS_SUM_AVX2_HADD_HPP
#define SIMD_ALGORITHMS_SUM_AVX2_HADD_HPP

#include <cstdint>

#ifdef ENABLE_SIMD_AVX2

namespace simd::algorithms::avx2 {

std::uint32_t sum_hadd(std::uint8_t * data, std::size_t size);

} // namespace simd::algorithms::avx2

#endif

#endif // SIMD_ALGORITHMS_SUM_AVX2_HADD_HPP
