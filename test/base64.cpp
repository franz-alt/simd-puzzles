#include <gtest/gtest.h>

#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

#include <libsimd/algorithms/base64/naive.hpp>
#include <libsimd/algorithms/base64/avx2/shuffle.hpp>

TEST(test_base64, to_base64_naive)
{
    // example taken from Wikipedia (https://en.wikipedia.org/wiki/Base64)

    std::string message = 
        "Man is distinguished, not only by his reason, but by this singular passion from other animals, "
        "which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable "
        "generation of knowledge, exceeds the short vehemence of any carnal pleasure.";

    std::string expected_result_message =
        "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlz"
        "IHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2Yg"
        "dGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGlu"
        "dWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRo"
        "ZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=";

    std::vector<std::uint8_t> data(message.begin(), message.end());

    const std::uint32_t length = data.size();
    const std::uint32_t result_length = static_cast<std::uint32_t>(std::ceil(length / 3.0 * 4.0));
    const std::uint32_t result_padding = (4 - result_length % 4) % 4;

    std::vector<std::uint8_t> result;
    result.resize(result_length + result_padding);

    simd::algorithms::to_base64_naive(&data[0], length, result_padding, &result[0]);

    std::string result_message(result.begin(), result.end());

    ASSERT_TRUE(expected_result_message == result_message);
}

TEST(test_base64, to_base64_naive_small_input)
{
    // modified example taken from Wikipedia (https://en.wikipedia.org/wiki/Base64)

    std::string message = 
        "Man is";

    std::string expected_result_message =
        "TWFuIGlz";

    std::vector<std::uint8_t> data(message.begin(), message.end());

    const std::uint32_t length = data.size();
    const std::uint32_t result_length = static_cast<std::uint32_t>(std::ceil(length / 3.0 * 4.0));
    const std::uint32_t result_padding = (4 - result_length % 4) % 4;

    std::vector<std::uint8_t> result;
    result.resize(result_length + result_padding);

    simd::algorithms::to_base64_naive(&data[0], length, result_padding, &result[0]);

    std::string result_message(result.begin(), result.end());

    ASSERT_TRUE(expected_result_message == result_message);
}

TEST(test_base64, to_base64_naive_empty_input)
{
    std::vector<std::uint8_t> data;

    const std::uint32_t length = data.size();
    const std::uint32_t result_length = static_cast<std::uint32_t>(std::ceil(length / 3.0 * 4.0));
    const std::uint32_t result_padding = (4 - result_length % 4) % 4;

    std::vector<std::uint8_t> result;
    result.resize(result_length + result_padding);

    simd::algorithms::to_base64_naive(&data[0], length, result_padding, &result[0]);

    std::string result_message(result.begin(), result.end());

    ASSERT_TRUE(result_message.empty());
}
#ifdef ENABLE_SIMD_AVX2
TEST(test_base64, to_base64_avx2_shuffle)
{
    // example taken from Wikipedia (https://en.wikipedia.org/wiki/Base64)

    std::string message = 
        "Man is distinguished, not only by his reason, but by this singular passion from other animals, "
        "which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable "
        "generation of knowledge, exceeds the short vehemence of any carnal pleasure.";

    std::string expected_result_message =
        "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlz"
        "IHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2Yg"
        "dGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGlu"
        "dWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRo"
        "ZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=";

    std::vector<std::uint8_t> data(message.begin(), message.end());

    const std::uint32_t length = data.size();
    const std::uint32_t result_length = static_cast<std::uint32_t>(std::ceil(length / 3.0 * 4.0));
    const std::uint32_t result_padding = (4 - result_length % 4) % 4;

    std::vector<std::uint8_t> result;
    result.resize(result_length + result_padding);

    simd::algorithms::avx2::to_base64_shuffle(&data[0], length, result_padding, &result[0]);

    std::string result_message(result.begin(), result.end());

    ASSERT_TRUE(expected_result_message == result_message);
}

TEST(test_base64, to_base64_avx2_shuffle_small_input)
{
    // modified example taken from Wikipedia (https://en.wikipedia.org/wiki/Base64)

    std::string message = 
        "Man is";

    std::string expected_result_message =
        "TWFuIGlz";

    std::vector<std::uint8_t> data(message.begin(), message.end());

    const std::uint32_t length = data.size();
    const std::uint32_t result_length = static_cast<std::uint32_t>(std::ceil(length / 3.0 * 4.0));
    const std::uint32_t result_padding = (4 - result_length % 4) % 4;

    std::vector<std::uint8_t> result;
    result.resize(result_length + result_padding);

    simd::algorithms::avx2::to_base64_shuffle(&data[0], length, result_padding, &result[0]);

    std::string result_message(result.begin(), result.end());

    ASSERT_TRUE(expected_result_message == result_message);
}

TEST(test_base64, to_base64_avx2_shuffle_empty_input)
{
    std::vector<std::uint8_t> data;

    const std::uint32_t length = data.size();
    const std::uint32_t result_length = static_cast<std::uint32_t>(std::ceil(length / 3.0 * 4.0));
    const std::uint32_t result_padding = (4 - result_length % 4) % 4;

    std::vector<std::uint8_t> result;
    result.resize(result_length + result_padding);

    simd::algorithms::avx2::to_base64_shuffle(&data[0], length, result_padding, &result[0]);

    std::string result_message(result.begin(), result.end());

    ASSERT_TRUE(result_message.empty());
}
#endif
