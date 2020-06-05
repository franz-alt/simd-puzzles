#include "naive.hpp"

namespace simd::algorithms {

std::uint32_t sum_naive(std::uint8_t * data, std::size_t size)
{
    std::uint32_t sum = 0;

    for (std::size_t i = 0; i < size; ++i)
    {
        sum += data[i];
    }

    return sum;
}

} // namespace simd::algorithms
