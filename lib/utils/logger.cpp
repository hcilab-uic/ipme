#include "logger.h"

#include <cstring>
#include <ctime>

namespace cdi {
namespace utils {

logging::sources::severity_logger<logging::trivial::severity_level> Logger::log;

/* static */ void Logger::init(const std::string& file_prefix)
{
    char buffer[16];
    memset(buffer, 0, sizeof(buffer));
    std::time_t time = std::time(nullptr);
    std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S",
                  std::localtime(&time));

    namespace src = logging::sources;
    namespace expr = logging::expressions;
    namespace kw = logging::keywords;

    logging::add_file_log(
        kw::file_name = file_prefix + "_" + std::string(buffer) + "_%N.log",
        kw::format =
            (expr::stream << expr::format_date_time<boost::posix_time::ptime>(
                                 "TimeStamp", "%Y%m%d %H:%M:%S")
                          << " <" << logging::trivial::severity << ">"
                          << "[" << expr::attr<std::string>(__FILE__)
                          << "]: " << expr::smessage));

    logging::add_common_attributes();
}
} // namespace utils
} // namespace cdi
