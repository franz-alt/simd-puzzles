#ifndef SIMD_ALGORITHMS_SUM_NAIVE_HPP
#define SIMD_ALGORITHMS_SUM_NAIVE_HPP

#include <cstdint>

namespace simd::algorithms {

std::uint32_t sum_naive(std::uint8_t * data, std::size_t size);

} // namespace simd::algorithms

#endif // SIMD_ALGORITHMS_SUM_NAIVE_HPP
