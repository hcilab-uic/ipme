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

#include "omicron_config.h"

#include <sstream>

#include "../data/trackable_factory.h"
#include "../utils/logger.h"

namespace ipme {
namespace utils {
Omicron_config::Omicron_config(io::Parser& parser) : parser_{parser}
{
    const auto ptree = parser_.parse();

    loglevel_ = ptree.get<std::string>("loglevel");
    hostname_ = ptree.get<std::string>("servername");
    kinect_only_mode_ = ptree.get<bool>("kinect_only_mode");
    port_ = ptree.get<short>("port");
    data_points_ = ptree.get<size_t>("data_points");

    for(const auto& object : ptree.get_child("trackable_objects")) {
        const auto type_desc = object.second.get<std::string>("type_desc");
        const size_t object_id = object.second.get<size_t>("object_id");
        auto object_ptr = data::trackable_factory::create(
            type_desc, std::to_string(object_id));

        objects_.push_back(object_ptr);
    }
}

std::string Omicron_config::to_string() const
{
    std::stringstream ss;
    ss << "Configuration\n";
    ss << "\t\t\t================================\n";
    ss << "\t\t\tHostname: " << hostname_ << "\n"
       << "\t\t\tPort: " << port_ << "\n"
       << "\t\t\tNumber of data points: " << data_points_
       << "\n\t\t\tObjects:\n";
    for(const auto& object : objects_) {
        ss << object->to_string() << "\n";
    }
    ss << "\t\t\t================================";

    return ss.str();
}

boost::log::trivial::severity_level Omicron_config::log_severity() const
{
    if(loglevel_ == "trace") {
        return boost::log::trivial::trace;
    } else if(loglevel_ == "debug") {
        return boost::log::trivial::debug;
    } else if(loglevel_ == "info") {
        return boost::log::trivial::info;
    } else if(loglevel_ == "warning") {
        return boost::log::trivial::warning;
    } else if(loglevel_ == "error") {
        return boost::log::trivial::error;
    } else if(loglevel_ == "fatal") {
        return boost::log::trivial::fatal;
    }

    WARN() << "Unknown log level: " << loglevel_ << ", logging at WARNING";

    return boost::log::trivial::warning;
}

} // namespace utils
} // namespace ipme
