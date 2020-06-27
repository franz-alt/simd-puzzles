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

std::size_t from_base64_shuffle(std::uint8_t * src, std::size_t length, std::uint8_t * dst)
{
    // check if padding characters ('=') are located at the end ; if yes, correct 'length'
    if (length > 0)
    {
        for (std::size_t i = length - 1; i >= 0; --i)
        {
            if (src[i] != '=')
            {
                length = i + 1;
                break;
            }
        }
    }

    std::size_t j = 0;

    for (std::size_t i = 0; i < length - (length % 32); i += 32, j += 24)
    {
        // load data from (unaligned) memory into 256-bit register
        __m256i raw = _mm256_lddqu_si256((__m256i const *)(src + i));

        //
        // create masks for all types of characters
        //

        // mask 'a' - 'z'
        __m256i t1 = _mm256_subs_epu8(raw, _mm256_set1_epi8('a' - 1));
        __m256i mask_a_z = _mm256_cmpeq_epi8(t1, _mm256_setzero_si256());
        mask_a_z = _mm256_xor_si256(mask_a_z, _mm256_set1_epi8(0xff));      // all 0xff that have values >= 'a' && <= 'z'

        // mask 'A' - 'Z'
        __m256i t2 = _mm256_subs_epu8(raw, _mm256_set1_epi8('A' - 1));
        __m256i t3 = _mm256_cmpgt_epi8(t2, _mm256_set1_epi8(26));           // all values > 'Z'
        __m256i t4 = _mm256_cmpeq_epi8(t2, _mm256_setzero_si256());         // all values < 'A'
        __m256i mask_A_Z = _mm256_or_si256(t3, t4);
        mask_A_Z = _mm256_xor_si256(mask_A_Z, _mm256_set1_epi8(0xff));      // all 0xff that have values >= 'A' && <= 'Z'

        // mask '0' - '9'
        __m256i t5 = _mm256_subs_epu8(raw, _mm256_set1_epi8('0' - 1));
        __m256i t6 = _mm256_cmpgt_epi8(t5, _mm256_set1_epi8(10));           // all values > '9'
        __m256i t7 = _mm256_cmpeq_epi8(t5, _mm256_setzero_si256());         // all values < '0'
        __m256i mask_0_9 = _mm256_or_si256(t6, t7);
        mask_0_9 = _mm256_xor_si256(mask_0_9, _mm256_set1_epi8(0xff));      // all 0x00 that have values >= '0' && <= '9'

        // mask '+'
        __m256i mask_plus = _mm256_cmpeq_epi8(raw, _mm256_set1_epi8('+'));  // all 0xff that have values == '+'

        // mask '/'
        __m256i mask_slash = _mm256_cmpeq_epi8(raw, _mm256_set1_epi8('/')); // all 0xff that have values == '/'

        //
        // subtract offsets which are added at encoding stage
        //

        // subtract offsets for all values of type 'a' - 'z'
        __m256i val_a_z = _mm256_sub_epi8(raw, _mm256_set1_epi8('a'));
        val_a_z = _mm256_add_epi8(val_a_z, _mm256_set1_epi8(26));
        val_a_z = _mm256_and_si256(val_a_z, mask_a_z);

        // subtract offsets for all values of type 'A' - 'Z'
        __m256i val_A_Z = _mm256_sub_epi8(raw, _mm256_set1_epi8('A'));
        val_A_Z = _mm256_and_si256(val_A_Z, mask_A_Z);

        // subtract offsets for all values of type '0' - '9'
        __m256i val_0_9 = _mm256_sub_epi8(raw, _mm256_set1_epi8('0'));
        val_0_9 = _mm256_add_epi8(val_0_9, _mm256_set1_epi8(52));
        val_0_9 = _mm256_and_si256(val_0_9, mask_0_9);

        // subtract offsets for all values of type '+'
        __m256i val_plus = _mm256_sub_epi8(raw, _mm256_set1_epi8('+'));
        val_plus = _mm256_and_si256(val_plus, mask_plus);

        // subtract offsets for all values of type '/'
        __m256i val_slash = _mm256_sub_epi8(raw, _mm256_set1_epi8('/'));
        val_slash = _mm256_and_si256(val_slash, mask_slash);

        // merge all offset corrected values together
        __m256i chars = _mm256_or_si256(val_a_z, val_A_Z);
        chars = _mm256_or_si256(chars, val_0_9);
        chars = _mm256_or_si256(chars, val_plus);
        chars = _mm256_or_si256(chars, val_slash);

        //
        // remap all types of characters from their 6- to their 8bit representations
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
        // bits : 543210543210543210543210
        // chars: |--4-||--3-||--2-||--1-|
        //

        // get byte #1 at position #1 of an int32
        __m256i t8 = _mm256_and_si256(chars, _mm256_set1_epi32(0x0000003f));
        t8 = _mm256_sll_epi32(t8, _mm_set_epi32(0, 0, 0, 2));
        __m256i t9 = _mm256_and_si256(chars, _mm256_set1_epi32(0x00003000));
        t9 = _mm256_srl_epi32(t9, _mm_set_epi32(0, 0, 0, 12));
        __m256i byte_1 = _mm256_or_si256(t8, t9);

        // get byte #2 at position #2 of an int32
        __m256i t10 = _mm256_and_si256(chars, _mm256_set1_epi32(0x00000f00));
        t10 = _mm256_sll_epi32(t10, _mm_set_epi32(0, 0, 0, 4));
        __m256i t11 = _mm256_and_si256(chars, _mm256_set1_epi32(0x003c0000));
        t11 = _mm256_srl_epi32(t11, _mm_set_epi32(0, 0, 0, 10));
        __m256i byte_2 = _mm256_or_si256(t10, t11);

        // get byte #3 at position #3 of an int32
        __m256i t12 = _mm256_and_si256(chars, _mm256_set1_epi32(0x00030000));
        t12 = _mm256_sll_epi32(t12, _mm_set_epi32(0, 0, 0, 6));
        __m256i t13 = _mm256_and_si256(chars, _mm256_set1_epi32(0x3f000000));
        t13 = _mm256_srl_epi32(t13, _mm_set_epi32(0, 0, 0, 8));
        __m256i byte_3 = _mm256_or_si256(t12, t13);

        // merge all bytes #1, #2 and #3 together
        __m256i res = _mm256_or_si256(byte_1, byte_2);
        res = _mm256_or_si256(res, byte_3);

        // reorder merged result to perform write in on step
        __m256i t14 = _mm256_shuffle_epi8(res, _mm256_set_epi8(0, 0, 0, 0, 14, 13, 12, 10, 9, 8, 6, 5, 4, 2, 1, 0,
                                                               0, 0, 0, 0, 14, 13, 12, 10, 9, 8, 6, 5, 4, 2, 1, 0));

        __m256i r = _mm256_permutevar8x32_epi32(t14, _mm256_set_epi32(7, 7, 6, 5, 4, 2, 1, 0));

        // write to destination array
        _mm256_maskstore_epi32((int *)(dst + j), _mm256_set_epi32(0, 0, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000), r);
    }

    auto convert =
        [](std::uint8_t & val)
        {
            if (val >= 'A' && val <= 'Z')
            {
                val = val - 'A';
            }
            else if (val >= 'a' && val <= 'z')
            {
                val = val - 'a' + 26;
            }
            else if (val >= '0' && val <= '9')
            {
                val = val - '0' + 52;
            }
            else if (val == '+')
            {
                val = 62;
            }
            else if (val == '/')
            {
                val = 63;
            }
        };

    for (std::size_t i = length - (length % 32); i != length - (length % 4); i += 4, j += 3)
    {
        std::uint8_t char_1 = src[i    ];
        std::uint8_t char_2 = src[i + 1];
        std::uint8_t char_3 = src[i + 2];
        std::uint8_t char_4 = src[i + 3];

        convert(char_1);
        convert(char_2);
        convert(char_3);
        convert(char_4);

        std::uint8_t byte_1 =  (char_1) << 2 | ((char_2 & 0b00110000) >> 4);
        std::uint8_t byte_2 = ((char_2 & 0b00001111) << 4) | ((char_3 & 0b00111100) >> 2);
        std::uint8_t byte_3 =  (char_3 & 0b00000011) << 6 | char_4;

        dst[j    ] = byte_1;
        dst[j + 1] = byte_2;
        dst[j + 2] = byte_3;
    }

    const std::size_t rest = length % 4;

    const std::size_t i = length - rest;

    if (rest == 1)
    {
        std::uint8_t char_1 = src[i];

        convert(char_1);

        std::uint8_t byte_1 = char_1 << 2;

        dst[j] = byte_1;

        j += 1 - 1;
    }
    else if (rest == 2)
    {
        std::uint8_t char_1 = src[i    ];
        std::uint8_t char_2 = src[i + 1];

        convert(char_1);
        convert(char_2);

        std::uint8_t byte_1 = (char_1) << 2 | ((char_2 & 0b00110000) >> 4);
        std::uint8_t byte_2 = (char_2 & 0b00001111) << 4;

        dst[j    ] = byte_1;
        dst[j + 1] = byte_2;

        j += 2 - 1;
    }
    else if (rest == 3)
    {
        std::uint8_t char_1 = src[i    ];
        std::uint8_t char_2 = src[i + 1];
        std::uint8_t char_3 = src[i + 2];

        convert(char_1);
        convert(char_2);
        convert(char_3);

        std::uint8_t byte_1 =  (char_1) << 2 | ((char_2 & 0b00110000) >> 4);
        std::uint8_t byte_2 = ((char_2 & 0b00001111) << 4) | ((char_3 & 0b00111100) >> 2);
        std::uint8_t byte_3 =  (char_3 & 0b00000011) << 6;

        dst[j    ] = byte_1;
        dst[j + 1] = byte_2;
        dst[j + 2] = byte_3;

        j += 3 - 1;
    }

    return j;
}

} // namespace simd::algorithms::avx2

#endif
