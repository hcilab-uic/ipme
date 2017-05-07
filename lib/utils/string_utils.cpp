#include "string_utils.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

namespace cdi {
namespace utils {
std::vector<std::string> split_string(const std::string& str,
                                      const std::string& delim)
{
    std::vector<std::string> split_vector;
    boost::split(split_vector, str, boost::is_any_of(delim),
                 boost::token_compress_on);
    return split_vector;
}

double to_double(const std::string& str)
{
    char* end;
    return std::strtod(str.c_str(), &end);
}
} // namespace utils
} // namespace cdi
