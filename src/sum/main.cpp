#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

#include <libbenchmark/testsuite.hpp>

#include <libsimd/algorithms/sum/naive.hpp>

#ifdef ENABLE_SIMD_AVX2

#include <libsimd/algorithms/sum/avx2/hadd.hpp>
#include <libsimd/algorithms/sum/avx2/sad.hpp>

#endif

const std::size_t repeats = 100;

benchmark::testrun perform_test(std::uniform_int_distribution<> & distribution, std::mt19937 & generator, std::size_t values)
{
	std::vector<std::uint8_t> data;
	data.reserve(values);

 	for (std::size_t n = 0; n < values; ++n)
	{
        data.push_back(distribution(generator));
    }

	benchmark::testrun tr(values);

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

	return tr;
}

int main(int argc, char * argv[])
{
	namespace po = boost::program_options;

	po::options_description general_options("general options");
	general_options.add_options()
		("help,h", "show this help text")
		("csv,c", "generate separate CSV files of performance results")
		("csv-all", "generate a single CSV file of all performance results")
	;

	po::options_description cmdline_options("usage: bench_sum [options]");
	cmdline_options.add(general_options);

	po::variables_map variables;
	po::store(po::parse_command_line(argc, argv, cmdline_options), variables);
	po::notify(variables);

	if (variables.count("help"))
	{
		std::cout << cmdline_options;
		return 1;
	}

	// create random numbers (inspired by https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution)

    std::random_device rd;  // will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // standard mersenne_twister_engine seeded with rd()

    std::uniform_int_distribution<> dist(0, 255);

	benchmark::testsuite ts("sum");

	for (std::size_t i = 4; i <= 15; ++i)
	{
		ts.add(perform_test(dist, gen, 1024 * std::pow(2, i)));
	}

	std::cout << ts.to_string() << std::endl;

	if (variables.count("csv"))
	{
		for (auto const & r : ts.to_csv())
		{
			std::string filename(ts.title());
			filename.append("_");
			filename.append(std::to_string(r.quantity));
			filename.append(".csv");
			std::replace(filename.begin(), filename.end(), ' ', '_');

			std::ofstream file;
			file.open(filename);
			file << r.data;
			file.close();
		}
	}

	if (variables.count("csv-all"))
	{
		auto r = ts.to_csv(false);

		if (!r.empty())
		{
			std::string filename(ts.title());
			filename.append("_all.csv");
			std::replace(filename.begin(), filename.end(), ' ', '_');

			std::ofstream file;
			file.open(filename);
			file << r.front().data;
			file.close();
		}
	}

	return 0;
}
