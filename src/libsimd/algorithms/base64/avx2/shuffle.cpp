#include "shuffle.hpp"

#ifdef ENABLE_SIMD_AVX2

#include <immintrin.h>

namespace simd::algorithms::avx2 {

void to_base64_shuffle(std::uint8_t * src, std::size_t length, std::size_t padding, std::uint8_t * dst)
{
    std::size_t j = 0;

    for (std::size_t i = 0; i < length - (length % 24); i += 24, j += 32)
    {
        // load data from (unaligned) memory into 256-bit register
        __m256i raw = _mm256_lddqu_si256((__m256i const *)(src + i));

        // reorder input data to get four triplets on each side of the 256-bit register
        __m256i t0 = _mm256_permutevar8x32_epi32(raw, _mm256_set_epi32(0, 5, 4, 3, 3, 2, 1, 0));

        // shuffle bytes to get the reordered 32-bit values to be on the right place
        __m256i t1 = _mm256_shuffle_epi8(t0, _mm256_set_epi8(0, 0, 0, 0, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
                                                             0, 0, 0, 0, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0));

        // filter out bytes that we're not interested in ...
        __m256i t2 = _mm256_and_si256(t0, _mm256_set_epi32(0, 0, 0, 0, 0, 0xffffffff, 0xffffffff, 0xffffffff));
        __m256i t3 = _mm256_and_si256(t1, _mm256_set_epi32(0, 0xffffffff, 0xffffffff, 0xffffffff, 0, 0, 0, 0));

        // ...to get the 'final' four triplets on each 128-bit side of the 256-bit register
        //
        // bits:      [255.........................................128|127...........................................0]
        // int32:     [32........0|32........0|32........0|32........0|32........0|32........0|32........0|32........0]
        // reordered: [xx|xx|xx|xx|B3|B2|B1|B3|B2|B1|B3|B2|B1|B3|B2|B1|xx|xx|xx|xx|B3|B2|B1|B3|B2|B1|B3|B2|B1|B3|B2|B1]
        //
        // xx = don't care
        //
        __m256i t4 = _mm256_or_si256(t2, t3);

        // reorder triplets again to perform character transformation
        //
        // bits:      [255.........................................128|127...........................................0]
        // int32:     [32........0|32........0|32........0|32........0|32........0|32........0|32........0|32........0]
        // reordered: [B3|B2|B1|B1|B3|B2|B1|B1|B3|B2|B1|B1|B3|B2|B1|B1|B3|B2|B1|B1|B3|B2|B1|B1|B3|B2|B1|B1|B3|B2|B1|B1]
        //
        __m256i t5 = _mm256_shuffle_epi8(t4, _mm256_set_epi8(11, 10, 9, 9,   8, 7, 6, 6,   5, 4, 3, 3,   2, 1, 0, 0,
                                                             11, 10, 9, 9,   8, 7, 6, 6,   5, 4, 3, 3,   2, 1, 0, 0));

        //
        // mapping of bytes to characters:
        //
        // bytes: |---3--||---2--||---1--|
        // bits : 765432107654321076543210
        // bits :                 543210
        // char :                 |--1-|
        // bits :         3210          54
        // char :         -2-|          |-
        // bits : 10          5432
        // char : -|          |--3
        // bits :   543210
        // char :   |--4-|
        //

        // get char #1 ; result on byte position #1 of an int32
        __m256i t6 = _mm256_and_si256(t5, _mm256_set1_epi32(0x000000fc));
        t6 = _mm256_srl_epi32(t6, _mm_set_epi32(0, 0, 0, 2));

        // get char #2 ; result on byte position #2 of an int32
        __m256i t7 = _mm256_and_si256(t5, _mm256_set1_epi32(0x00f00300));
        t7 = _mm256_shuffle_epi8(t7, _mm256_set_epi8(12, 13, 14, 12,   8, 9, 10, 8,   4, 5, 6, 4,   0, 1, 2, 0,
                                                     12, 13, 14, 12,   8, 9, 10, 8,   4, 5, 6, 4,   0, 1, 2, 0));
        t7 = _mm256_srl_epi32(t7, _mm_set_epi32(0, 0, 0, 4));

        // get char #3 ; result on byte position #3 of an int32
        __m256i t8 = _mm256_and_si256(t5, _mm256_set1_epi32(0xc00f0000));
        t8 = _mm256_shuffle_epi8(t8, _mm256_set_epi8(14, 15, 12, 12,   10, 11, 8, 8,   6, 7, 4, 4,   2, 3, 0, 0,
                                                     14, 15, 12, 12,   10, 11, 8, 8,   6, 7, 4, 4,   2, 3, 0, 0));
        t8 = _mm256_srl_epi32(t8, _mm_set_epi32(0, 0, 0, 6));

        // get char #4 ; result on byte position #4 of an int32
        __m256i t9 = _mm256_and_si256(t5, _mm256_set1_epi32(0x3f000000));

        // bitwise-OR all intermediate results together to get final result
        __m256i t10 = _mm256_or_si256(t6, t7);
        __m256i t11 = _mm256_or_si256(t8, t9);
        __m256i t12 = _mm256_or_si256(t10, t11);

        // subtract '25'
        __m256i offset1 = _mm256_set1_epi8(25);

        __m256i to1 = _mm256_subs_epu8(t12, offset1);
        __m256i to2 = _mm256_cmpeq_epi8(to1, _mm256_setzero_si256());   // all 0x00 that have values <= 25

        // map values in range [0,25] = val + 'A'
        __m256i res1 = _mm256_add_epi8(t12, _mm256_set1_epi8('A'));
        res1 = _mm256_and_si256(res1, to2);

        // subtract further '26'
        __m256i offset2 = _mm256_set1_epi8(26);

        __m256i to3 = _mm256_or_si256(to1, to2);
        __m256i to4 = _mm256_subs_epu8(to3, offset2);
        __m256i to5 = _mm256_cmpeq_epi8(to4, _mm256_setzero_si256());   // all 0x00 that have values > 25 && <= 51

        // map values in range [26,51] = 'a' + (val - 26)
        __m256i res2 = _mm256_add_epi8(t12, _mm256_set1_epi8('a'));
        res2 = _mm256_sub_epi8(res2, _mm256_set1_epi8(26));
        res2 = _mm256_and_si256(res2, to5);

        // subtract further '10'
        __m256i offset3 = _mm256_set1_epi8(10);

        __m256i to6 = _mm256_or_si256(to4, to5);
        __m256i to7 = _mm256_subs_epu8(to6, offset3);   
        __m256i to8 = _mm256_cmpeq_epi8(to7, _mm256_setzero_si256());   // all 0x00 that have values > 51 && <= 61

        // map values in range [52,61] = '0' + (val - 52))
        __m256i res3 = _mm256_add_epi8(t12, _mm256_set1_epi8('0'));
        res3 = _mm256_sub_epi8(res3, _mm256_set1_epi8(52));
        res3 = _mm256_and_si256(res3, to8);

        // subtract further '1'
        __m256i offset4 = _mm256_set1_epi8(1);

        __m256i to9 = _mm256_or_si256(to7, to8);
        __m256i to10 = _mm256_subs_epu8(to9, offset4);
        __m256i to11 = _mm256_cmpeq_epi8(to10, _mm256_setzero_si256());   // all 0x00 that have values == 62

        // map values in range [62] = '+'
        __m256i res4 = _mm256_set1_epi8('+');
        res4 = _mm256_and_si256(res4, to11);

        // subtract further '1'
        __m256i offset5 = _mm256_set1_epi8(1);

        __m256i to12 = _mm256_or_si256(to10, to11);
        __m256i to13 = _mm256_subs_epu8(to12, offset5);
        __m256i to14 = _mm256_cmpeq_epi8(to13, _mm256_setzero_si256());   // all 0x00 that have values == 63

        // map values in range [63] = '/'
        __m256i res5 = _mm256_set1_epi8('/');
        res5 = _mm256_and_si256(res5, to14);

        // build final result
        __m256i r = _mm256_or_si256(res1, res2);
        r = _mm256_or_si256(r, res3);
        r = _mm256_or_si256(r, res4);
        r = _mm256_or_si256(r, res5);

        // write to destination array
        _mm256_storeu_si256((__m256i *)(dst + j), r);
    }

    // handle remaining bytes in a non-vectorized way
    auto convert =
        [](std::uint8_t & val)
        {
            if (val <= 25)
            {
                val = 'A' + val;
            }
            else if (val > 25 && val <= 51)
            {
                val = 'a' + (val - 26);
            }
            else if (val > 51 && val <= 61)
            {
                val = '0' + (val - 52);
            }
            else if (val == 62)
            {
                val = '+';
            }
            else if (val == 63)
            {
                val = '/';
            }
        };

    for (std::size_t i = length - (length % 24); i != length - (length % 3); i += 3, j += 4)
    {
        const std::uint8_t & byte_1 = src[i    ];
        const std::uint8_t & byte_2 = src[i + 1];
        const std::uint8_t & byte_3 = src[i + 2];

        std::uint8_t char_1 =  (byte_1 & 0b11111100) >> 2;
        std::uint8_t char_2 = ((byte_1 & 0b00000011) << 4) | ((byte_2 & 0b11110000) >> 4);
        std::uint8_t char_3 = ((byte_2 & 0b00001111) << 2) | ((byte_3 & 0b11000000) >> 6);
        std::uint8_t char_4 =   byte_3 & 0b00111111;

        convert(char_1);
        convert(char_2);
        convert(char_3);
        convert(char_4);

        dst[j    ] = char_1;
        dst[j + 1] = char_2;
        dst[j + 2] = char_3;
        dst[j + 3] = char_4;
    }

    const std::size_t rest = length % 3;

    const std::size_t i = length - rest;

    if (rest == 1)
    {
        const std::uint8_t & byte_1 = src[i];

        std::uint8_t char_1 = (byte_1 & 0b11111100) >> 2;
        std::uint8_t char_2 = (byte_1 & 0b00000011) << 4;

        convert(char_1);
        convert(char_2);

        dst[j    ] = char_1;
        dst[j + 1] = char_2;

        j += 2;
    }
    else if (rest == 2)
    {
        const std::uint8_t & byte_1 = src[i    ];
        const std::uint8_t & byte_2 = src[i + 1];

        std::uint8_t char_1 =  (byte_1 & 0b11111100) >> 2;
        std::uint8_t char_2 = ((byte_1 & 0b00000011) << 4) | ((byte_2 & 0b11110000) >> 4);
        std::uint8_t char_3 =  (byte_2 & 0b00001111) << 2;

        convert(char_1);
        convert(char_2);
        convert(char_3);

        dst[j    ] = char_1;
        dst[j + 1] = char_2;
        dst[j + 2] = char_3;

        j += 3;
    }   

    for (std::size_t i = 0; i < padding; ++i, ++j)
    {
        dst[j] = '=';
    }
}

} // namespace simd::algorithms::avx2

#endif
