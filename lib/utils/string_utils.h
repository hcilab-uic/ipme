#ifndef CDI_LIB_UTILS_STRING_UTILS_H
#define CDI_LIB_UTILS_STRING_UTILS_H

#include <string>
#include <vector>

namespace cdi {
namespace utils {
/**
 * @brief split_string a string into vector by delimiter
 * @param str string to be split
 * @param delim delimiter (default: ,)
 * @return vector of split tokens
 */
std::vector<std::string> split_string(const std::string& str,
                                      const std::string& delim = ",");

/**
 * @brief to_double convert string to double
 * @param str string to be split
 * @return converted double
 */
double to_double(const std::string& str);
} // namespace utils
} // namespace cdi

#endif // CDI_LIB_UTILS_STRING_UTILS_H
