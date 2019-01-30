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

#ifndef IPME_UTILS_OMICRON_CONFIG_H
#define IPME_UTILS_OMICRON_CONFIG_H

#include <memory>
#include <string>
#include <vector>

#include "../data/trackable_object.h" // FIXME
#include "../io/parser.h"             // FIXME
#include "config.h"

namespace ipme {
namespace utils {
class Omicron_config : public Config {
public:
    /**
     * @brief Omicron_config taking reference to parser
     */
    Omicron_config(io::Parser& parser);

    /** @copydoc Config::to_string()
     */
    virtual std::string to_string() const override;

    /**
     * @brief hostname accessor function for server hostname
     * @return std::string hostname (eg: cave2tracker.evl.uic.edu)
     */
    inline std::string hostname() const
    {
        return hostname_;
    }

    /**
     * @brief port accessor to get the server to connect to
     * @return integer port
     */
    inline short port() const
    {
        return port_;
    }

    /**
     * @brief data_points
     * @return count of the data points to be collected
     */
    inline size_t data_points() const
    {
        return data_points_;
    }

    /**
     * @brief objects accessor to get all the configured objects
     * @return vector of objects
     */
    inline std::vector<std::shared_ptr<data::Trackable_object>> objects() const
    {
        return objects_;
    }

    /** @copydoc Config::log_severity() const
     */
    virtual boost::log::trivial::severity_level log_severity() const override;

    /**
     * @brief kinect_only_mode mode for ignoring VICON data
     * @return true if kinect only mode is enabled
     */
    inline bool kinect_only_mode() const
    {
        return kinect_only_mode_;
    }

private:
    io::Parser& parser_;
    std::string loglevel_;
    std::string hostname_;
    bool kinect_only_mode_ = false;
    short port_ = 0;
    size_t data_points_ = 0;
    std::vector<std::shared_ptr<data::Trackable_object>> objects_;
};
} // namespace utils
} // namespace ipme

#endif // IPME_UTILS_OMICRON_CONFIG_H
