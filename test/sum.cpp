#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

#include <libsimd/algorithms/sum/naive.hpp>
#include <libsimd/algorithms/sum/avx2/add.hpp>
#include <libsimd/algorithms/sum/avx2/hadd.hpp>
#include <libsimd/algorithms/sum/avx2/sad.hpp>

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

TEST(test_sum, sum_naive)
{
    const std::size_t n = 32 * 7;

    auto data = generate_ascending_data(n);
    auto sum = simd::algorithms::sum_naive(&data[0], n);

    ASSERT_TRUE(sum == ((n * (n + 1)) / 2));
}
#ifdef ENABLE_SIMD_AVX2
TEST(test_sum, sum_avx2_hadd)
{
    const std::size_t n = 32 * 7;

    auto data = generate_ascending_data(n);
    auto sum = simd::algorithms::avx2::sum_hadd(&data[0], n);

    ASSERT_TRUE(sum == ((n * (n + 1)) / 2));
}

TEST(test_sum, sum_avx2_add)
{
    const std::size_t n = 32 * 7;

    auto data = generate_ascending_data(n);
    auto sum = simd::algorithms::avx2::sum_add(&data[0], n);

    ASSERT_TRUE(sum == ((n * (n + 1)) / 2));
}

TEST(test_sum, sum_avx2_sad)
{
    const std::size_t n = 32 * 7;

    auto data = generate_ascending_data(n);
    auto sum = simd::algorithms::avx2::sum_sad(&data[0], n);

    ASSERT_TRUE(sum == ((n * (n + 1)) / 2));
}
#endif
