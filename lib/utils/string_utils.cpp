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

#include "string_utils.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

namespace ipme {
namespace utils {
std::vector<std::string> split_string(const std::string& str,
                                      const std::string& delim)
{
    std::vector<std::string> split_vector;
    boost::split(split_vector, str, boost::is_any_of(delim),
                 boost::token_compress_on);
    return split_vector;
}

double to_double(const std::string& str)
{
    char* end;
    return std::strtod(str.c_str(), &end);
}
} // namespace utils
} // namespace ipme
