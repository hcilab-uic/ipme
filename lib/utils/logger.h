#ifndef CDI_LIB_UTILS_LOGGER_H
#define CDI_LIB_UTILS_LOGGER_H

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>

#define DO_LOG(level) BOOST_LOG_SEV(ipme::utils::Logger::log, level)

#define TRACE() DO_LOG(boost::log::trivial::trace)
#define DEBUG() DO_LOG(boost::log::trivial::debug)
#define INFO() DO_LOG(boost::log::trivial::info)
#define WARN() DO_LOG(boost::log::trivial::info)
#define ERROR() DO_LOG(boost::log::trivial::info)
#define FATAL() DO_LOG(boost::log::trivial::info)

namespace ipme {
namespace utils {
namespace logging = boost::log;
class Logger {
  public:
    static void init(const std::string& file_prefix = "cdi");
    static logging::sources::severity_logger<logging::trivial::severity_level>
        log;

  private:
};

} // namespace utils
} // namespace ipme

#endif // CDI_LIB_UTILS_LOGGER_H
