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

#ifndef CDI_LIB_UTILS_STRING_UTILS_H
#define CDI_LIB_UTILS_STRING_UTILS_H

#include <string>
#include <vector>

namespace ipme {
namespace utils {
/**
 * @brief split_string a string into vector by delimiter
 * @param str string to be split
 * @param delim delimiter (default: ,)
 * @return vector of split tokens
 */
std::vector<std::string> split_string(const std::string& str,
                                      const std::string& delim = ",");

/**
 * @brief to_double convert string to double
 * @param str string to be split
 * @return converted double
 */
double to_double(const std::string& str);
} // namespace utils
} // namespace ipme

#endif // CDI_LIB_UTILS_STRING_UTILS_H
