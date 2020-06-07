#ifndef BENCHMARK_TESTSUITE_HPP
#define BENCHMARK_TESTSUITE_HPP

#include <string>
#include <vector>

#include "testrun.hpp"

namespace benchmark {

class testsuite
{
public:
    testsuite(std::string title);

    std::string title() const;

    void add(testrun run);

    std::string to_string() const;

    struct csv_result
    {
        csv_result(std::size_t quantity_, std::string data_)
            : quantity(std::move(quantity_))
            , data(std::move(data_))
        {}

        std::size_t quantity;
        std::string data;
    };

    std::vector<csv_result> to_csv(bool separate = true) const;

private:
    std::string m_title;

    std::vector<testrun> m_testruns;
};

} // namespace benchmark

#endif // BENCHMARK_TESTSUITE_HPP
