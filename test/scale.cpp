#include <gtest/gtest.h>

#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <vector>

#include <libsimd/algorithms/scale/naive.hpp>
#include <libsimd/algorithms/scale/avx2/fma.hpp>

namespace {

std::vector<std::uint8_t> generate_ascending_data(std::size_t size)
{
    std::vector<std::uint8_t> data;
    data.reserve(size);

    for (std::size_t i = 1; i <= size; ++i)
    {
        data.push_back(i);
    }

    return data;
}

}

TEST(test_scale, scale_naive)
{
    const std::size_t n = 32 * 7;

    auto src = generate_ascending_data(n);
    decltype(src) dst(src.size());

    simd::algorithms::scale_naive(&src[0], n, &dst[0], 3.1415, 42);

    // create expected vector with std-algorithm
    decltype(src) dst_expected(src.size());

    std::transform(src.begin(),
                   src.end(),
                   dst_expected.begin(),
                   [](auto const & v)
                   {
                       const double n = std::round(v * 3.1415 + 42);

                       return n > 255.0 ? std::numeric_limits<typename std::remove_reference<decltype(v)>::type>::max()
                                        : static_cast<typename std::remove_reference<decltype(v)>::type>(n);
                   });

    ASSERT_TRUE(dst == dst_expected);
}

TEST(test_scale, scale_naive_small_input)
{
    const std::size_t n = 9;

    auto src = generate_ascending_data(n);
    decltype(src) dst(src.size());

    simd::algorithms::scale_naive(&src[0], n, &dst[0], 3.1415, 42);

    // create expected vector with std-algorithm
    decltype(src) dst_expected(src.size());

    std::transform(src.begin(),
                   src.end(),
                   dst_expected.begin(),
                   [](auto const & v)
                   {
                       const double n = std::round(v * 3.1415 + 42);

                       return n > 255.0 ? std::numeric_limits<typename std::remove_reference<decltype(v)>::type>::max()
                                        : static_cast<typename std::remove_reference<decltype(v)>::type>(n);
                   });

    ASSERT_TRUE(dst == dst_expected);
}
#ifdef ENABLE_SIMD_AVX2
TEST(test_scale, scale_avx2_fma)
{
    const std::size_t n = 32 * 7;

    auto src = generate_ascending_data(n);
    decltype(src) dst(src.size());

    simd::algorithms::avx2::scale_fma(&src[0], n, &dst[0], 3.1415, 42);

    // create expected vector with std-algorithm
    decltype(src) dst_expected(src.size());

    std::transform(src.begin(),
                   src.end(),
                   dst_expected.begin(),
                   [](auto const & v)
                   {
                       const double n = std::round(v * 3.1415 + 42);

                       return n > 255.0 ? std::numeric_limits<typename std::remove_reference<decltype(v)>::type>::max()
                                        : static_cast<typename std::remove_reference<decltype(v)>::type>(n);
                   });

    ASSERT_TRUE(dst == dst_expected);
}

TEST(test_scale, scale_avx2_fma_small_input)
{
    const std::size_t n = 9;

    auto src = generate_ascending_data(n);
    decltype(src) dst(src.size());

    simd::algorithms::avx2::scale_fma(&src[0], n, &dst[0], 3.1415, 42);

    // create expected vector with std-algorithm
    decltype(src) dst_expected(src.size());

    std::transform(src.begin(),
                   src.end(),
                   dst_expected.begin(),
                   [](auto const & v)
                   {
                       const double n = std::round(v * 3.1415 + 42);

                       return n > 255.0 ? std::numeric_limits<typename std::remove_reference<decltype(v)>::type>::max()
                                        : static_cast<typename std::remove_reference<decltype(v)>::type>(n);
                   });

    ASSERT_TRUE(dst == dst_expected);
}
#endif