#ifndef BENCHMARK_TESTRUN_HPP
#define BENCHMARK_TESTRUN_HPP

#include <string>
#include <vector>

#include "result.hpp"

namespace benchmark {

class testrun
{
public:
    testrun(std::size_t quantity);

    std::size_t quantity() const;

    template<typename... Args>
    void add_result(Args... args)
    {
        m_results.emplace_back(std::forward<Args>(args)...);
    }

    std::vector<result> const & results() const;

    std::string to_string() const;

    std::string to_csv() const;

private:
    std::size_t m_quantity;

    std::vector<result> m_results;
};

} // namespace benchmark

#endif // BENCHMARK_TESTRUN_HPP
