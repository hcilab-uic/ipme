#ifndef IPME_UTILS_UTILS_H
#define IPME_UTILS_UTILS_H

#include <string>

#define UNUSED(x) (__attribute__((unused))(x))

namespace ipme {
namespace utils {
/*!
 * A function that takes a strfmt value and converts into a time stamp string
 * \param[in] fmt format (eg: "%Y%m%d-%H%M%S")
 * \return converted value (eg:  20170608-183854)
 */
std::string create_timestamp_string(const std::string& fmt);
} // namespace utils
} // namespace ipme

#endif // IPME_UTILS_UTILS_H
