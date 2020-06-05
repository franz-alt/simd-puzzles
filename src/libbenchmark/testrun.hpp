#ifndef BENCHMARK_TESTRUN_HPP
#define BENCHMARK_TESTRUN_HPP

#include <string>
#include <vector>

#include "result.hpp"

namespace benchmark {

class testrun
{
public:
    testrun(std::string title);

    template<typename... Args>
    void add_result(Args... args)
    {
        m_results.emplace_back(std::forward<Args>(args)...);
    }

    std::string to_string() const;

    std::string to_csv() const;

private:
    std::string m_title;

    std::vector<result> m_results;
};

} // namespace benchmark

#endif // BENCHMARK_TESTRUN_HPP
