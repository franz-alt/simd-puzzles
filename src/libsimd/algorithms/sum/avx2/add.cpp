#include "add.hpp"

#ifdef ENABLE_SIMD_AVX2

#include <immintrin.h>
#include <tmmintrin.h>

namespace simd::algorithms::avx2 {

std::uint32_t sum_add(std::uint8_t * data, std::size_t size)
{
    __m256i sum = _mm256_set1_epi8(0);;

    const __m256i and16_mask = _mm256_set1_epi16(0x00ff);
    const __m256i and32_mask = _mm256_set1_epi32(0x0000ffff);
    
    const __m256i zero = _mm256_setzero_si256();
    __m256i accu = zero;

    for (std::size_t i = 0; i < size; i += 32)
    {
        // load data from (unaligned) memory into 256-bit register
        __m256i raw = _mm256_loadu_si256((__m256i const *)(data + i));

        // clear every 2nd 8-bit value to build 16-bit values
        __m256i t0 = _mm256_and_si256(raw, and16_mask);

        // shift loaded data as 16-bit values to the right
        __m256i t1 = _mm256_srli_epi16(raw, 8);

        // accumulate the 16-bit values
        accu = _mm256_add_epi16(zero, t0);
        accu = _mm256_add_epi16(accu, t1);

        // clear every 2nd 16-bit value at accumulated result to build 32-bit values
        __m256i t2 = _mm256_and_si256(accu, and32_mask);

        // shift accumulated result as 32-bit values to the right
        __m256i t3 = _mm256_srli_epi32(accu, 16);

        // add the 32-bit values
        __m256i t4 = _mm256_add_epi32(t2, t3);

        // extract intermediate results
        sum += _mm256_extract_epi32(t4, 0) +
               _mm256_extract_epi32(t4, 1) +
               _mm256_extract_epi32(t4, 2) +
               _mm256_extract_epi32(t4, 3) +
               _mm256_extract_epi32(t4, 4) +
               _mm256_extract_epi32(t4, 5) +
               _mm256_extract_epi32(t4, 6) +
               _mm256_extract_epi32(t4, 7);
    }

    return _mm256_extract_epi32(sum, 0);
}

} // namespace simd::algorithms::avx2

#endif
