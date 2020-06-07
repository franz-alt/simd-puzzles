#include "testsuite.hpp"

#include <sstream>

namespace {

template<typename T>
std::string to_string(std::vector<T> const & data, std::string separator = ",")
{
    std::stringstream ss;

    if (!data.empty())
    {
        ss << data.front();

        for (auto it = data.begin() + 1; it != data.end(); ++it)
        {
            ss << separator << *it;
        }
    }

    return ss.str();
}

}

namespace benchmark {

testsuite::testsuite(std::string title)
    : m_title(std::move(title))
    , m_testruns()
{}

std::string testsuite::title() const
{
    return m_title;
}

void testsuite::add(testrun run)
{
    m_testruns.push_back(std::move(run));
}

std::string testsuite::to_string() const
{
    std::stringstream ss;
    ss << "testsuite: '" << m_title << "'" << std::endl << std::endl;

    for (auto const & tr : m_testruns)
    {
        ss << tr.to_string() << std::endl;
    }

    return ss.str();;
}

std::vector<testsuite::csv_result> testsuite::to_csv(bool separate) const
{
    std::vector<testsuite::csv_result> results;

    if (separate)
    {
        results.reserve(m_testruns.size());

        for (auto const & tr : m_testruns)
        {
            results.emplace_back(tr.quantity(), tr.to_csv());
        }
    }
    else
    {
        std::stringstream ss;

        bool print_header = true;

        for (auto const & tr : m_testruns)
        {
            std::vector<std::string> titles;
            std::vector<double> microseconds;

            for (auto const & r : tr.results())
            {
                titles.push_back(r.title());
                microseconds.push_back(r.microseconds());
            }

            if (print_header)
            {
                ss << "quantity," << ::to_string(titles) << std::endl;
                print_header = false;
            }

            ss << tr.quantity() << "," << ::to_string(microseconds) << std::endl;
        }

        results.emplace_back(0, ss.str());
    }
    
    return results;
}

} // namespace benchmark
