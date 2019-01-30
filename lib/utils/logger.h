/* A tool for collecting, visualizing and annotating 3D Interaction data
 *
 * Copyright (C) 2017-2019 University of Illinois at Chicago
 *
 * Author: Harish G. Naik <hnaik2@uic.edu>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef CDI_LIB_UTILS_LOGGER_H
#define CDI_LIB_UTILS_LOGGER_H

#include <cstring>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>

#define FILENAME (boost::filesystem::path(__FILE__)).filename().string()

#define DO_LOG(level)                                                          \
    BOOST_LOG_SEV(ipme::utils::Logger::log, level)                             \
        << "[" << FILENAME << ":" << __LINE__ << ":" << __func__ << "] "

#define TRACE() DO_LOG(boost::log::trivial::trace)
#define DEBUG() DO_LOG(boost::log::trivial::debug)
#define INFO() DO_LOG(boost::log::trivial::info)
#define WARN() DO_LOG(boost::log::trivial::warning)
#define ERROR() DO_LOG(boost::log::trivial::error)
#define FATAL() DO_LOG(boost::log::trivial::fatal)

namespace ipme {
namespace utils {

namespace logging = boost::log;
namespace trivial = logging::trivial;

class Logger {
public:
    /**
     * @brief init initialize logging
     * @param file_prefix prefix of the log file name
     */
    static void init(const std::string& file_prefix = "ipme");

    static void set_severity(const trivial::severity_level severity);

    static logging::sources::severity_logger<trivial::severity_level> log;

private:
};

} // namespace utils
} // namespace ipme

#endif // CDI_LIB_UTILS_LOGGER_H
