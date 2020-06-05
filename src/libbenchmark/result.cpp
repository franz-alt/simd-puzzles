#include "result.hpp"

namespace benchmark {

result::result(std::string title, double microseconds)
    : m_title(std::move(title))
    , m_microseconds(microseconds)
{}

std::string result::title() const
{
    return m_title;
}

double result::microseconds() const
{
    return m_microseconds;
}

} // namespace benchmark
