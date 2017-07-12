#include "logger.h"

#include <cstring>
#include <ctime>

#include "utils.h"

namespace ipme {
namespace utils {

logging::sources::severity_logger<logging::trivial::severity_level> Logger::log;

/* static */ void Logger::init(const std::string& file_prefix)
{
    namespace attrs = logging::attributes;
    namespace src = logging::sources;
    namespace expr = logging::expressions;
    namespace kw = logging::keywords;

    logging::add_file_log(
        kw::file_name = file_prefix + "_" +
                        create_timestamp_string("%Y%m%d_%H%M%S") + "_%N.log",
        kw::format = (expr::stream
                      << expr::format_date_time<boost::posix_time::ptime>(
                             "TimeStamp", "%Y%m%d %H:%M:%S")
                      << " " << std::left << std::setw(8) << std::setfill(' ')
                      << logging::trivial::severity << " " << expr::smessage));

    logging::add_common_attributes();
}
} // namespace utils
} // namespace ipme
