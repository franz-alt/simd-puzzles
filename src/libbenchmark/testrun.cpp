#include "testrun.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>

namespace benchmark {

testrun::testrun(std::string title)
    : m_title(std::move(title))
    , m_results()
{}

std::string testrun::to_string() const
{
    std::stringstream ss;
    ss << "testrun: '" << m_title << "'" << std::endl << std::endl;

    double reference_value = 0.0;

    if (!m_results.empty())
    {
        reference_value = m_results.front().microseconds();
    }

    // determine maximum lengths of result metrics
    std::size_t max_result_title_length = 0;
    std::size_t max_result_duration_length = 0;
    std::size_t max_result_speedup_length = 0;

    for (auto const & result : m_results)
    {
        max_result_title_length = std::max(static_cast<std::size_t>(9), std::max(max_result_title_length, result.title().size())); // 9 = length of 'procedure'
        max_result_duration_length = std::max(static_cast<std::size_t>(7), std::max(max_result_duration_length, std::to_string(static_cast<int>(result.microseconds())).size())); // 7 = length of 'average'
        max_result_speedup_length = std::max(static_cast<std::size_t>(7), std::max(max_result_speedup_length, std::to_string(static_cast<int>((reference_value / result.microseconds()))).size())); // 7 = length of 'speedup'
    }

    // add some space between the columns
    max_result_title_length += 3;
    max_result_duration_length += 2;
    max_result_speedup_length += 1;

    ss << std::setfill(' ') << std::setw(max_result_title_length) << std::left << "procedure" << " "
       << std::setfill(' ') << std::setw(max_result_duration_length) << std::right << "average" << "    "
       << std::setfill(' ') << std::setw(max_result_speedup_length) << std::right << "speedup" << std::endl;

    ss << std::setfill('-') << std::setw(max_result_title_length) << std::left << "-" << " "
       << std::setfill('-') << std::setw(max_result_duration_length + 3) << std::left << "-" << " " // +3 = for unit at end
       << std::setfill('-') << std::setw(max_result_speedup_length) << std::left << "-" << std::endl;

    for (auto const & result : m_results)
    {
        ss << std::setfill(' ') << std::setw(max_result_title_length) << std::left << result.title() << " "
           << std::setfill(' ') << std::setw(max_result_duration_length) << std::right << std::setprecision(2) << std::fixed << result.microseconds() << " us "
           << std::setfill(' ') << std::setw(max_result_speedup_length) << std::right << std::setprecision(2) << std::fixed << (reference_value / result.microseconds()) << std::endl;
    }

    return ss.str();
}

std::string testrun::to_csv() const
{
    std::stringstream ss;
    ss << "Procedure,Average (in us),Speedup" << std::endl;

    double reference_value = 0.0;

    if (!m_results.empty())
    {
        reference_value = m_results.front().microseconds();
    }

    for (auto const & result : m_results)
    {
        ss << result.title() << ","
           << std::setprecision(2) << std::fixed << result.microseconds() << ","
           << std::setprecision(2) << std::fixed << (reference_value / result.microseconds()) << std::endl;
    }

    return ss.str();
}

} // namespace benchmark
