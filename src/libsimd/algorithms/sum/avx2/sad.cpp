#include "sad.hpp"

#ifdef ENABLE_SIMD_AVX2

#include <immintrin.h>

namespace simd::algorithms::avx2 {

std::uint32_t sum_sad(std::uint8_t * data, std::size_t size)
{
    std::uint32_t sum = 0;

    const __m256i zeros = _mm256_set1_epi8(0);

    for (std::size_t i = 0; i < size; i += 32)
    {
        // load data from (unaligned) memory into 256-bit register
        __m256i raw = _mm256_loadu_si256((__m256i const *)(data + i));

        // compute absolute differences
        __m256i tmp = _mm256_sad_epu8(raw, zeros);

        // add up extracted parts of 'tmp'
        sum += _mm256_extract_epi32(tmp, 0) +
               _mm256_extract_epi32(tmp, 2) +
               _mm256_extract_epi32(tmp, 4) +
               _mm256_extract_epi32(tmp, 6);
    }

    return sum;
}

} // namespace simd::algorithms::avx2

#endif
