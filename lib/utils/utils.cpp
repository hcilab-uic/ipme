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

#include "utils.h"

#include <cassert>
#include <cstring>
#include <ctime>

namespace ipme {
namespace utils {

std::string create_timestamp_string(const std::string& fmt)
{
    static constexpr size_t k_buffer_length = 128;
    assert(fmt.size() < k_buffer_length);

    char buffer[k_buffer_length];
    memset(buffer, 0, sizeof(buffer));
    std::time_t time = std::time(nullptr);
    std::strftime(buffer, sizeof(buffer), fmt.c_str(), std::localtime(&time));

    return buffer;
}

} // namespace utils
} // namespace ipme
