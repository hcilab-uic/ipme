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
} // namespace ipme

#endif // IPME_UTILS_CONFIG_H
