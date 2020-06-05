#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

#include <libbenchmark/testrun.hpp>

#include <libsimd/algorithms/sum/naive.hpp>

#ifdef ENABLE_SIMD_AVX2

#include <libsimd/algorithms/sum/avx2/hadd.hpp>
#include <libsimd/algorithms/sum/avx2/sad.hpp>

#endif

int main()
{
	// create random numbers (inspired by https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution)

    std::random_device rd;  // will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // standard mersenne_twister_engine seeded with rd()

    std::uniform_int_distribution<> dist(0, 255);

	const std::size_t values = 4096 * 8 * 100;
	const std::size_t repeats = 1000;

	std::vector<std::uint8_t> data;
	data.reserve(values);

 	for (std::size_t n = 0; n < values; ++n)
	{
        data.push_back(dist(gen));
    }

	benchmark::testrun tr("sum");

	// naive without SIMD
	{
		std::vector<std::chrono::microseconds> durations;
		durations.reserve(repeats);

		for (std::size_t i = 0; i < repeats; ++i)
		{
			auto start = std::chrono::system_clock::now();

			simd::algorithms::sum_naive(&data[0], data.size());

			auto stop = std::chrono::system_clock::now();

			durations.push_back(std::chrono::duration_cast<std::chrono::microseconds>(stop - start));
		}

		auto sum = std::accumulate(durations.begin(),
								   durations.end(),
								   static_cast<std::size_t>(0),
								   [](std::size_t sum, auto duration)
								   {
									   return sum + duration.count();
								   });

		tr.add_result("naive", sum / static_cast<double>(repeats));
	}

#ifdef ENABLE_SIMD_AVX2
	// AVX2 with 'hadd' instruction
	{
		std::vector<std::chrono::microseconds> durations;
		durations.reserve(repeats);

		for (std::size_t i = 0; i < repeats; ++i)
		{
			auto start = std::chrono::system_clock::now();

			simd::algorithms::avx2::sum_hadd(&data[0], data.size());

			auto stop = std::chrono::system_clock::now();

			durations.push_back(std::chrono::duration_cast<std::chrono::microseconds>(stop - start));
		}

		auto sum = std::accumulate(durations.begin(),
								   durations.end(),
								   static_cast<std::size_t>(0),
								   [](std::size_t sum, auto duration)
								   {
									   return sum + duration.count();
								   });

		tr.add_result("AVX2 (hadd)", sum / static_cast<double>(repeats));
	}

	// AVX2 with 'sad' instruction
	{
		std::vector<std::chrono::microseconds> durations;
		durations.reserve(repeats);

		for (std::size_t i = 0; i < repeats; ++i)
		{
			auto start = std::chrono::system_clock::now();

			simd::algorithms::avx2::sum_sad(&data[0], data.size());

			auto stop = std::chrono::system_clock::now();

			durations.push_back(std::chrono::duration_cast<std::chrono::microseconds>(stop - start));
		}

		auto sum = std::accumulate(durations.begin(),
								   durations.end(),
								   static_cast<std::size_t>(0),
								   [](std::size_t sum, auto duration)
								   {
									   return sum + duration.count();
								   });

		tr.add_result("AVX2 (sad)", sum / static_cast<double>(repeats));
	}
#endif

	std::cout << tr.to_string() << std::flush;

	return 0;
}
