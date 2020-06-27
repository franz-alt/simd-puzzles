#include "naive.hpp"

namespace simd::algorithms {

void to_base64_naive(std::uint8_t * src, std::size_t length, std::size_t padding, std::uint8_t * dst)
{
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

    std::size_t j = 0;

    for (std::size_t i = 0; i < length - (length % 3); i += 3, j += 4)
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

std::size_t from_base64_naive(std::uint8_t * src, std::size_t length, std::uint8_t * dst)
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

    std::size_t j = 0;

    for (std::size_t i = 0; i < length - (length % 4); i += 4, j += 3)
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

} // namespace simd::algorithms
