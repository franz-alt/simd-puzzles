#include "fma.hpp"

#include <cmath>

#ifdef ENABLE_SIMD_AVX2

#include <immintrin.h>

namespace {

__m128i fma(__m128i const & data, __m256d const & factors, __m256d const & offsets)
{
    //
    // to perform a fused-multiply-add we have to:
    // 1. convert the 8-bit integers to 32-bit double precision floating numbers
    // 2. perform multiplication and addition
    // 3. convert the (saturated) values back to 8-bit integers
    //

    // convert 8- to 16-bit integer values
    const __m256i conv16 = _mm256_cvtepu8_epi16(data);

    // extract high and low 128-bit parts of the converted 16-bit integers
    const __m128i conv16_lo = _mm256_extracti128_si256(conv16, 0);
    const __m128i conv16_hi = _mm256_extracti128_si256(conv16, 1);

    // convert 16- to 32-bit integers
    const __m256i conv32_lo = _mm256_cvtepi16_epi32(conv16_lo);
    const __m256i conv32_hi = _mm256_cvtepi16_epi32(conv16_hi);

    const __m128i conv32_lolo = _mm256_extracti128_si256(conv32_lo, 0);
    const __m128i conv32_lohi = _mm256_extracti128_si256(conv32_lo, 1);
    const __m128i conv32_hilo = _mm256_extracti128_si256(conv32_hi, 0);
    const __m128i conv32_hihi = _mm256_extracti128_si256(conv32_hi, 1);

    // convert 32-bit integers to 64-bit doubles
    const __m256d real_lolo = _mm256_cvtepi32_pd(conv32_lolo);
    const __m256d real_lohi = _mm256_cvtepi32_pd(conv32_lohi);
    const __m256d real_hilo = _mm256_cvtepi32_pd(conv32_hilo);
    const __m256d real_hihi = _mm256_cvtepi32_pd(conv32_hihi);

    // perform multiplications and additions
    const __m256d fma_lolo = _mm256_fmadd_pd(real_lolo, factors, offsets);
    const __m256d fma_lohi = _mm256_fmadd_pd(real_lohi, factors, offsets);
    const __m256d fma_hilo = _mm256_fmadd_pd(real_hilo, factors, offsets);
    const __m256d fma_hihi = _mm256_fmadd_pd(real_hihi, factors, offsets);

    // convert 64-bit result to 32-bit integers
    const __m128i res_lolo = _mm256_cvtpd_epi32(fma_lolo);
    const __m128i res_lohi = _mm256_cvtpd_epi32(fma_lohi);
    const __m128i res_hilo = _mm256_cvtpd_epi32(fma_hilo);
    const __m128i res_hihi = _mm256_cvtpd_epi32(fma_hihi);

    const __m256i res_lo = _mm256_set_m128i(res_lohi, res_lolo);
    const __m256i res_hi = _mm256_set_m128i(res_hihi, res_hilo);

    // saturate (min/max) result
    const __m256i saturated_lo = _mm256_min_epu32(_mm256_max_epu32(res_lo, _mm256_setzero_si256()), _mm256_set1_epi32(255));
    const __m256i saturated_hi = _mm256_min_epu32(_mm256_max_epu32(res_hi, _mm256_setzero_si256()), _mm256_set1_epi32(255));

    // shuffle saturated 8-bit values
    const __m256i shuffled_lo = _mm256_shuffle_epi8(saturated_lo, _mm256_set_epi8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 8, 4, 0,
                                                                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 8, 4, 0));
    const __m256i shuffled_hi = _mm256_shuffle_epi8(saturated_hi, _mm256_set_epi8(0, 0, 0, 0, 0, 0, 0, 0, 12, 8, 4, 0, 0, 0, 0, 0,
                                                                                  0, 0, 0, 0, 0, 0, 0, 0, 12, 8, 4, 0, 0, 0, 0, 0));

    // filter out all values that are not needed ...
    const __m256i filt_lo = _mm256_and_si256(shuffled_lo, _mm256_set_epi32(0, 0, 0, 0xffffffff, 0, 0, 0, 0xffffffff));
    const __m256i filt_hi = _mm256_and_si256(shuffled_hi, _mm256_set_epi32(0, 0, 0xffffffff, 0, 0, 0, 0xffffffff, 0));

    // ... and merge filter masks
    const __m256i filt = _mm256_or_si256(filt_hi, filt_lo);

    // rearrange bytes
    const __m256i permuted = _mm256_permutevar8x32_epi32(filt, _mm256_set_epi32(0, 0, 0, 0, 5, 1, 4, 0));

    // we're only interested in the lower half of the 256-bit register
    return _mm256_castsi256_si128(permuted);
}

}

namespace simd::algorithms::avx2 {

void scale_fma(std::uint8_t * src, std::size_t length, std::uint8_t * dst, double factor, std::uint8_t offset)
{
    const __m256d factors = _mm256_set1_pd(factor);
    const __m256d offsets = _mm256_set1_pd(static_cast<double>(offset));

    for (std::size_t i = 0; i < length - (length % 32); i += 32)
    {
        // load data from (unaligned) memory into 256-bit register
        const __m256i raw = _mm256_lddqu_si256((__m256i const *)(src + i));

        // split loaded 256-bit data into two 128-bit parts
        const __m128i raw_lo = _mm256_extracti128_si256(raw, 0);
        const __m128i raw_hi = _mm256_extracti128_si256(raw, 1);

        // perform multiplication and addition at once
        const __m128i fma_lo = fma(raw_lo, factors, offsets);
        const __m128i fma_hi = fma(raw_hi, factors, offsets);

        // pack high/low results together
        const __m256i fma = _mm256_set_m128i(fma_hi, fma_lo);

        // write to destination array
        _mm256_storeu_si256((__m256i *)(dst + i), fma);
    }

    for (std::size_t i = length - (length % 32); i != length; ++i)
    {
        dst[i] = std::round(src[i] * factor + offset);
    }
}

} // namespace simd::algorithms::avx2

#endif
