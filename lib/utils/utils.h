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
