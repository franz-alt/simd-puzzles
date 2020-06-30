#include "naive.hpp"

#include <cmath>

namespace simd::algorithms {

void scale_naive(std::uint8_t * src, std::size_t length, std::uint8_t * dst, double factor, std::uint8_t offset)
{
    for (std::size_t i = 0; i < length; ++i)
    {
        dst[i] = static_cast<std::uint8_t>(std::min(std::round(src[i] * factor + offset), 255.0));
    }
}

} // namespace simd::algorithms
