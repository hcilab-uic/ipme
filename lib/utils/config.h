#ifndef IPME_UTILS_CONFIG_H
#define IPME_UTILS_CONFIG_H

#include <boost/log/trivial.hpp>
#include <string>

namespace ipme {
namespace utils {
class Config {
  public:
    /**
     * virtual destructor
     */
    virtual ~Config() = default;

    /**
     * @brief to_string return a custom string representation of the object
     * @return stringified object
     */
    virtual std::string to_string() const = 0;

    /**
     * @brief log_severity return severity level as configured
     * @return severityh level
     */
    inline virtual boost::log::trivial::severity_level log_severity() const
    {
        return boost::log::trivial::warning;
    }
};
} // namespace utils
} // ipme

#endif // IPME_UTILS_CONFIG_H
