#include "to_string.hpp"

#include <cstdint>
#include <iomanip>
#include <sstream>

#ifdef ENABLE_SIMD_AVX2

namespace simd::utils {

std::string to_string(__m128i const & data, std::string const & description, bool generate_header, bool descending)
{
  std::stringstream ss;

  if (generate_header)
  {
      ss << std::string(description.size() + 2, ' ');

      if (descending)
      {
          ss << "[127...........................................0]" << std::endl;
      }
      else
      {
          ss << "[0...........................................127]" << std::endl;
      }
  }

  if (!description.empty())
  {
      ss << description << ": ";
  }

  const std::uint8_t * raw = reinterpret_cast<const std::uint8_t *>(&data);

  ss << "[";

  for (std::uint8_t i = 0; i < 16; ++i)
  {
      ss << std::setfill('0') << std::setw(2) << std::right << std::hex << static_cast<int>(raw[descending ? (31 - i) : i]);

      if (i < 15)
      {
          ss << "|";
      }
  }

  ss << "]" << std::endl;

  return ss.str();
}

std::string to_string(__m256i const & data, std::string const & description, bool generate_header, bool descending)
{
    std::stringstream ss;

    if (generate_header)
    {
        ss << std::string(description.size() + 2, ' ');

        if (descending)
        {
            ss << "[255.........................................128|127...........................................0]" << std::endl;
        }
        else
        {
            ss << "[0...........................................127|128.........................................255]" << std::endl;
        }
    }

    if (!description.empty())
    {
        ss << description << ": ";
    }

    const std::uint8_t * raw = reinterpret_cast<const std::uint8_t *>(&data);

    ss << "[";

    for (std::uint8_t i = 0; i < 32; ++i)
    {
        ss << std::setfill('0') << std::setw(2) << std::right << std::hex << static_cast<int>(raw[descending ? (31 - i) : i]);

        if (i < 31)
        {
            ss << "|";
        }
    }

    ss << "]" << std::endl;

    return ss.str();
}

} // namespace simd::utils

#endif
