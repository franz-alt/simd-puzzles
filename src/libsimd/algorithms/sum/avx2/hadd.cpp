#include "hadd.hpp"

#ifdef ENABLE_SIMD_AVX2

#include <immintrin.h>
#include <tmmintrin.h>

namespace simd::algorithms::avx2 {

std::uint32_t sum_hadd(std::uint8_t * data, std::size_t size)
{
    const __m128i shuffle_mask = _mm_setr_epi8(8, 9, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0);

    auto partial_sum =
        [&shuffle_mask](__m128 const & data, __m128i & result)
        {
                    // convert lower half of 128-bit register of 'data' from 8-bit values to 32-bit values
            __m256i converted_low = _mm256_cvtepu8_epi32(data);

            // shuffle data to process upper half of 128-bit register of 'data'
            __m128i data_shuffled = _mm_shuffle_epi8(data, shuffle_mask);

            // convert upper half of 128-bit register of 'data' from 8-bit values to 32-bit values
            __m256i converted_high = _mm256_cvtepu8_epi32(data_shuffled);

            // perform two times a horizontal addition ...
            __m256i tmp1_hadd = _mm256_hadd_epi32(converted_high, converted_high);
            __m256i tmp2_hadd = _mm256_hadd_epi32(tmp1_hadd, tmp1_hadd);

            // ... extract two 128-bit from 256-bit register ...
            __m128i tmp2_hadd_high = _mm256_extracti128_si256(tmp2_hadd, 1);
            __m128i tmp2_hadd_low  = _mm256_extracti128_si256(tmp2_hadd, 0);

            // ... followed by an ordinary addition to get the partial sum of 'converted_high'
            __m128i part_sum_high = _mm_add_epi32(tmp2_hadd_high, tmp2_hadd_low);

            // perform two times a horizontal addition ...
            tmp1_hadd = _mm256_hadd_epi32(converted_low, converted_low);
            tmp2_hadd = _mm256_hadd_epi32(tmp1_hadd, tmp1_hadd);

            // ... extract two 128-bit from 256-bit register ...
            tmp2_hadd_high = _mm256_extracti128_si256(tmp2_hadd, 1);
            tmp2_hadd_low  = _mm256_extracti128_si256(tmp2_hadd, 0);

            // ... followed by an ordinary addition to get the partial sum of 'converted_low'
            __m128i part_sum_low = _mm_add_epi32(tmp2_hadd_high, tmp2_hadd_low);

            // finally perform an ordinary addition to get the final partial sum
            result = _mm_add_epi32(part_sum_high, part_sum_low);
        };

    __m256i hsum = _mm256_set1_epi8(0);;

    for (std::size_t i = 0; i < size; i += 32)
    {
        // load data from (unaligned) memory into 256-bit register
        __m256i raw = _mm256_loadu_si256((__m256i const *)(data + i));

        // extract two 128-bit from 256-bit register
        __m128i raw_high = _mm256_extracti128_si256(raw, 1);
        __m128i raw_low  = _mm256_extracti128_si256(raw, 0);

        // perform partial sum on high and low parts
        __m128i part_sum_high;
        __m128i part_sum_low;

        partial_sum(raw_high, part_sum_high);
        partial_sum(raw_low, part_sum_low);

        // calculate the final sum of 'data' that represents the sum of the pixels of horizontal filter for line at 'y'
        __m128i sum = _mm_add_epi32(part_sum_high, part_sum_low);

        // increment vertical sum variable
        hsum = _mm256_add_epi32(hsum, _mm256_set_m128i(_mm_set1_epi8(0), sum));
    }

    return _mm256_extract_epi32(hsum, 0);
}

} // namespace simd::algorithms::avx2

#endif
