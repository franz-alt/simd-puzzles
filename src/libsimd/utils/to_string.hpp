#ifndef SIMD_UTILS_TO_STRING_HPP
#define SIMD_UTILS_TO_STRING_HPP

#ifdef ENABLE_SIMD_AVX2

#include <immintrin.h>

#endif

#include <string>

#ifdef ENABLE_SIMD_AVX2

namespace simd::utils {

std::string to_string(__m128i const & data, std::string const & description = "", bool generate_header = false);
std::string to_string(__m256i const & data, std::string const & description = "", bool generate_header = false);

} // namespace simd::utils

#endif

#endif // SIMD_UTILS_TO_STRING_HPP
