#ifndef BENCHMARK_RESULT_HPP
#define BENCHMARK_RESULT_HPP

#include <string>

namespace benchmark {

class result
{
public:
    result(std::string title = "", double microseconds = 0.0);

    std::string title() const;

    double microseconds() const;

private:
    std::string m_title;
    double m_microseconds;
};

} // namespace benchmark

#endif // BENCHMARK_RESULT_HPP
